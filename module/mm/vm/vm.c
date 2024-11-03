#include <mm/vm.h>
#include <mm/slab.h>
#include <aco/log.h>
#include <aco/errno.h>
#include <aco/assert.h>
#include <mm/buddy.h>
#include <aco/string.h>

#include <vma_area_ops.h>
#include <vm_internal.h>

int	mmap_kernel_io_hw_cb(vma_t* vma, vma_area_t* vma_area)
{
	int ret;
	ret = hw_mmap(vma->hw_pgtable, vma_area->va, vma_area->pa, vma_area->prot, vma_area->size);
	ASSERT(ret != -EEXIST);
	return ret;
}
typedef int (*mmap_hw_cb)(vma_t*, vma_area_t*);
static mmap_hw_cb  hw_map_callbacks[]  =
{
	[VMA_KERNEL] 	= mmap_kernel_io_hw_cb,
	[VMA_IO]	= mmap_kernel_io_hw_cb,
};

int  __vm_mmap(vma_t* vma, viraddr_t va, phyaddr_t pa, vm_prot prot, uintptr_t size, vmar_type_t type, bool need_hwmap)
{
        ASSERT(vma != NULL && size >0 \
			&& VAL_IS_PAGE_ALIGNED(va)\
			&& VAL_IS_PAGE_ALIGNED(pa)\
			&& VAL_IS_PAGE_ALIGNED(size));
        ASSERT(type >= VMA_KERNEL && type < VMA_NOREACH);
	int ret = 0;
        vma_area_t* vma_area = kmem_obj_alloc(vma_area_cache);
	if(vma_area == NULL)
                return -ENOBUFS;
        vma_area_val_init(vma_area, type, va, pa,size,NULL,prot);
        w_lock_br(&(vma->rwlk));
        ret = vma_area_hang(vma, vma_area);
	if(ret)
		goto free_vma_area;
	if(need_hwmap)
        {
		ASSERT(hw_map_callbacks[type] != NULL);
                ret = hw_map_callbacks[type](vma, vma_area);
                if(ret)
			goto unhang_vma_area;
        }
        w_unlock_br(&(vma->rwlk));
        return 0;
unhang_vma_area:
	vma_area_unhang(vma, vma_area);
free_vma_area:
	kmem_obj_free(vma_area_cache, vma_area);
	w_unlock_br(&(vma->rwlk));
	return ret;
}

struct find_helper {
        viraddr_t va;
        uintptr_t size;
};
static int find_compare(struct find_helper* key, struct avl_node* have)
{
        vma_area_t* have_node = container_of(have, vma_area_t, a_node);
        if(key->va == have_node->va && key->size == have_node->size)
                return 0;
        if(key->va >= have_node->va + have_node->size)
                return 1;
        if(key->va + key->size <= have_node->va)
                return -1;
        return -1;
}
static void hw_unmap_kernel_io(vma_t* vma, vma_area_t* vma_area)
{
	hw_unmap(vma->hw_pgtable, vma_area->va, vma_area->size);
}
typedef void (*unmap_hw_cb)(vma_t*, vma_area_t*);
static unmap_hw_cb hw_unmap_callbacks[] = 
{
	[VMA_KERNEL] 	= hw_unmap_kernel_io,
	[VMA_IO]	= hw_unmap_kernel_io,
};
int __vm_unmap(vma_t* vma, viraddr_t va, uintptr_t size, vmar_type_t type, int need_hwunmap)
{
        ASSERT(vma != NULL && size >0 \
                        && VAL_IS_PAGE_ALIGNED(va) \
                        && VAL_IS_PAGE_ALIGNED(size));
        struct find_helper fd_hp;
        fd_hp.va = va;
        fd_hp.size = size;
        w_lock_br(&(vma->rwlk));
        struct avl_node* res = NULL;
        avl_node_find(&(vma->a_root), &fd_hp, find_compare, res);
        if(res == NULL)
        {
                w_unlock_br(&(vma->rwlk));
                return -ENOBUFS;
        }
        vma_area_t* vma_area = container_of(res, vma_area_t, a_node);
        ASSERT(vma_area->va == va && vma_area->size == size);
        if(vma_area->vmar_typ != type)
        {
                w_unlock_br(&(vma->rwlk));
                return -EPERM;
        }
	vma_area_unhang(vma, vma_area);
        if(need_hwunmap)
	{
		ASSERT(hw_unmap_callbacks[type] != NULL);
		hw_unmap_callbacks[type](vma, vma_area);
	}
	kmem_obj_free(vma_area_cache, vma_area);
        w_unlock_br(&(vma->rwlk));
        return 0;
}

vma_t*  vma_alloc()
{
	vma_t* ret = kmem_obj_alloc(vma_cache);
	if(ret == NULL)
		return ret;
	ASSERT(ret->hw_pgtable == 0 && ret->kernel_vma == 0);
	ret->hw_pgtable = (pgt*)bk_alloc(0);
	if(ret->hw_pgtable == NULL)
		goto free_vma_cache;
	rwlock_init(&(ret->rwlk));
	ret->kernel_vma = kern_vma;
	avl_root_init(&(ret->a_root));
	INIT_LIST_HEAD(&(ret->l_list));
	uintptr_t seperate_line = (uintptr_t)get_seperate_line();
        uintptr_t seperate_size = seperate_line - PGFRAME_VA_START;
        if(__vm_mmap(ret, PGFRAME_VA_START, PGFRAME_PA_START, PROT_V | PROT_X | PROT_R, seperate_size, VMA_KERNEL, 1) != 0)
		goto free_map_area;
	if(__vm_mmap(ret, PGFRAME_VA_START + seperate_size, PGFRAME_PA_START + seperate_size, \
				PROT_V | PROT_W | PROT_R, PGFRAME_SIZE - seperate_size, VMA_KERNEL, 1) != 0)
		goto free_map_area;
	return ret;
free_map_area:
	vma_free(ret);
	return NULL;
free_vma_cache:
	kmem_obj_free(vma_cache, ret);
	return NULL;
}

void    vma_free(vma_t* vma)
{
	if(vma == NULL)
		return;
	if(vma == KVMA)
	{
		log_warn("[VMA] : try to free kernel vma");
		return;
	}
	if(vma->kernel_vma != KVMA)
		return;
	struct list_head* iterator;
	struct list_head* safe;
	w_lock_br(&(vma->rwlk));
	list_for_each_safe(iterator, safe, &(vma->l_list))
	{
		vma_area_t* vma_area = list_entry(iterator, vma_area_t, l_node);
		vma_area_unhang(vma, vma_area);
		ASSERT(hw_unmap_callbacks[vma_area->vmar_typ] != NULL);
                hw_unmap_callbacks[vma_area->vmar_typ](vma, vma_area);
		kmem_obj_free(vma_area_cache, vma_area);
	}
	hw_unmap_all(vma->hw_pgtable); // to free all hw_pgtable index alloc
	w_unlock_br(&(vma->rwlk));
	memset(vma,0,sizeof(vma_t));
	kmem_obj_free(vma_cache, vma);
	return;
}

int     vm_iomap(vma_t* vma, viraddr_t va, phyaddr_t pa, vm_prot prot, uintptr_t size)
{
	if(!((va >= PGFRAME_VA_END) || (va + size <= PGFRAME_VA_START)))
                return -EPERM;
	if(!((pa >= PGFRAME_PA_END) || (pa + size <= PGFRAME_PA_START)))
		return -EPERM;
	if(vma == NULL)
		return -EFAULT;
	if(!((va & PAGE_ALIGN_BIT) == (pa & PAGE_ALIGN_BIT))\
			|| size <=0)
		return -EFAULT;
	if(!VAL_IS_PAGE_ALIGNED(va))
		return -EFAULT;
	size = PAGE_UPPER_ALIGN(size);
	return __vm_mmap(vma, va, pa, prot, size, VMA_IO, 1);
}

int     vm_iounmap(vma_t* vma, viraddr_t va, uintptr_t size)
{
	if(!((va >= PGFRAME_VA_END) || (va + size <= PGFRAME_VA_START)))
		return -EPERM;
	if(vma == NULL)
                return -EFAULT;
	if(!VAL_IS_PAGE_ALIGNED(va))
                return -EFAULT;
	size = PAGE_UPPER_ALIGN(size);
	return __vm_unmap(vma, va, size, VMA_IO, 1);
}

int     vm_mmap(vma_t* vma, viraddr_t va, pageframe_t* pgfm, vm_prot prot)
{
	return 0;
}

int     vm_unmap(vma_t* vma, viraddr_t va)
{
	return 0;
}

int     vma_cache_number()
{
	kmm_cinfo info;
	kmem_cached_info(vma_cache,&info);
	return info.obj_num;
}

int     vma_area_cache_number()
{
	kmm_cinfo info;
        kmem_cached_info(vma_area_cache,&info);
	return info.obj_num;
}

int     vm_check(vma_t* vma)
{
	if(vma == NULL)
		return -EFAULT;
	r_lock(&(vma->rwlk));
	if(vma->hw_pgtable == NULL)
		return -EFAULT;
	if(vma->kernel_vma != kern_vma)
		return -EFAULT;
	struct list_head* iterator;
	viraddr_t bef = (viraddr_t)-1;
	uintptr_t size = (uintptr_t)-1;
	list_for_each(iterator, &(vma->l_list))
	{
		vma_area_t* vma_area = list_entry(iterator, vma_area_t, l_node);
		if(!(VAL_IS_PAGE_ALIGNED(vma_area->va) && VAL_IS_PAGE_ALIGNED(vma_area->pa)\
				&& VAL_IS_PAGE_ALIGNED(vma_area->size)\
				&& vma_area->size > 0))
		{
			r_unlock(&(vma->rwlk));
			return -EFAULT;
		}
		if(vma_area->vmar_typ == VMA_IO || vma_area->vmar_typ == VMA_KERNEL)
		{
			if(vma_area->pgfm != NULL)
			{
				r_unlock(&(vma->rwlk));
				return -EFAULT;
			}
		}
		if(!(vma_area->prot & PROT_V))
		{
			r_unlock(&(vma->rwlk));
			return -EFAULT;
		}
		if(bef == (viraddr_t)-1)
		{
			bef = vma_area->va;
			size = vma_area->size;
		}
		else
		{
			if(!(vma_area->va >= bef +size))
			{
				r_unlock(&(vma->rwlk));
				return -EFAULT;
			}
			bef = vma_area->va;
			size = vma_area->size;
		}
		struct find_helper fd_hp;
		fd_hp.va = vma_area->va;
		fd_hp.size = vma_area->size;
		struct avl_node* res = NULL;
		avl_node_find(&(vma->a_root), &fd_hp, find_compare, res);
		if(res == NULL || container_of(res, vma_area_t, a_node) != vma_area)
		{
			r_unlock(&(vma->rwlk));
			return -EFAULT;
		}
	}
	r_unlock(&(vma->rwlk));
	return 0;
}

void    vm_dump_information(vma_t* vma, output_callback out_cb)
{
	ASSERT(vma != NULL && out_cb != NULL);
	r_lock(&(vma->rwlk));
	out_cb("[VMA ] hw_pgtable : 0x%p\n", vma->hw_pgtable);
	out_cb("[VMA ] kenrne_vma : 0x%p\n", vma->kernel_vma);
	struct list_head* iterator;
        list_for_each(iterator, &(vma->l_list))
	{
		 vma_area_t* vma_area = list_entry(iterator, vma_area_t, l_node);
		 if(vma_area->vmar_typ == VMA_KERNEL)
			 out_cb("[VMA ] vma_type :   VMA_KERNEL\n");
		 else if(vma_area->vmar_typ == VMA_IO)
			 out_cb("[VMA ] vma_type :   VMA_IO\n");
		 else if(vma_area->vmar_typ == VMA_MEMORY)
			 out_cb("[VMA ] vma_type :   VMA_MEMORY\n");
		 out_cb("[VMA ] vma_area va: 0x%p\n",vma_area->va);
		 out_cb("[VMA ] vma_area pa: 0x%p\n",vma_area->pa);
		 out_cb("[VMA ] vma_area sz: 0x%p\n",vma_area->size);
		 out_cb("[VMA ] vma_area pf: 0x%p\n",vma_area->pgfm);
		 out_cb("[VMA ] area permin: ");
		 if(vma_area->prot & PROT_R)
			 out_cb("r");
		 else
			 out_cb("-");
		 if(vma_area->prot & PROT_W)
                         out_cb("w");
                 else
                         out_cb("-");
		 if(vma_area->prot & PROT_X)
                         out_cb("x");
                 else
                         out_cb("-");
		 if(vma_area->prot & PROT_V)
                         out_cb("v");
                 else
                         out_cb("-");
		 if(vma_area->prot & PROT_U)
                         out_cb("u\n");
                 else
                         out_cb("-\n");
	}
	r_unlock(&(vma->rwlk));
}
