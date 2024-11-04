#include <mm/vm.h>
#include <mm/slab.h>
#include <aco/log.h>
#include <aco/errno.h>
#include <aco/assert.h>
#include <mm/buddy.h>
#include <aco/string.h>

#include <vma_area_ops.h>
#include <vm_internal.h>

static int mmap_kernel_io_hw_cb(vma_t* vma, vma_area_t* vma_area)
{
	int ret;
	ret = hw_mmap(vma->hw_pgtable, vma_area->va, vma_area->pa, vma_area->prot, vma_area->size);
	ASSERT(ret != -EEXIST);
	return ret;
}
static int mmap_memory_cb(vma_t* vma, vma_area_t* vma_area)
{
	int ret;
	ASSERT(vma_area->pgfm == NULL);
	if(vma_area->pa == (phyaddr_t)-1)
	{
		//lazy allocation
		//not do hwmap and pgfm set
		//return success for default
		return 0;
	}
	ASSERT(pa_valid(vma_area->pa));
	ret = hw_mmap(vma->hw_pgtable, vma_area->va, vma_area->pa, vma_area->prot, vma_area->size);
        ASSERT(ret != -EEXIST);
	if(ret != 0)
		return ret;
	pageframe_t* pgfm =  pa2pg(vma_area->pa);
	lock(&(pgfm->lk));
	ASSERT(pgfm->pgtype == VMA_AREA \
			&& (pgfm->meta).buddy_flags == PG_BUDDY_FLAG_HEAD\
			&& (1<<((pgfm->meta).buddy_order))*PAGE_SIZE == vma_area->size);
	(pgfm->meta_other).ref_count++;
	unlock(&(pgfm->lk));
	vma_area->pgfm = pgfm;
	return 0;
}
typedef int (*mmap_hw_cb)(vma_t*, vma_area_t*);
static mmap_hw_cb  hw_map_callbacks[]  =
{
	[VMA_KERNEL] 	= mmap_kernel_io_hw_cb,
	[VMA_IO]	= mmap_kernel_io_hw_cb,
	[VMA_MEMORY]    = mmap_memory_cb,
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
static void unmmap_memory_cb(vma_t* vma, vma_area_t* vma_area)
{
	if(vma_area->pa == (phyaddr_t)-1)
	{
		// if free a lazy area
		// just do nothing
		ASSERT(vma_area->pgfm == NULL);
		return;
	}
	pageframe_t* pgfm = vma_area->pgfm;
	ASSERT(pgfm != NULL);
	lock(&(pgfm->lk));
	ASSERT(pgfm->pgtype == VMA_AREA \
                        && (pgfm->meta).buddy_flags == PG_BUDDY_FLAG_HEAD\
                        && (1<<((pgfm->meta).buddy_order))*PAGE_SIZE == vma_area->size);
	(pgfm->meta_other).ref_count--;
	if((pgfm->meta_other).ref_count == 0)
	{
		pgfm->pgtype = PAGE_BUDDYALLOCATOR;
		unlock(&(pgfm->lk));
		bkp_free(pgfm);
	}
	else
		unlock(&(pgfm->lk));
	vma_area->pgfm = NULL;
	hw_unmap(vma->hw_pgtable, vma_area->va, vma_area->size);
	return;
}
typedef void (*unmap_hw_cb)(vma_t*, vma_area_t*);
static unmap_hw_cb hw_unmap_callbacks[] = 
{
	[VMA_KERNEL] 	= hw_unmap_kernel_io,
	[VMA_IO]	= hw_unmap_kernel_io,
	[VMA_MEMORY]	= unmmap_memory_cb,
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
	return vma_dup(KVMA);
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

int     vm_mmap(vma_t* vma, viraddr_t va, int order, vm_prot prot)
{
	if(vma == NULL || !VAL_IS_PAGE_ALIGNED(va)\
			|| !(order >=0 && order < MAX_ORDER))
		return -EFAULT;
	int ret;
	pageframe_t* pgfm = bkp_alloc_zero(order);
	if(pgfm == NULL)
		return -ENOMEM;
	lock(&(pgfm->lk));
	ASSERT((pgfm->meta).buddy_order == order\
			&& (pgfm->meta).buddy_flags == PG_BUDDY_FLAG_HEAD\
			&& pgfm->pgtype == PAGE_BUDDYALLOCATOR);
	pgfm->pgtype = VMA_AREA;
	(pgfm->meta_other).ref_count = 0;
	(pgfm->meta_other).vma_flags = 0;
	unlock(&(pgfm->lk));
	ret = __vm_mmap(vma, va, pg2pa(pgfm), prot, (1<<order)*PAGE_SIZE, VMA_MEMORY, 1);
	if(ret)
	{
		lock(&(pgfm->lk));
		pgfm->pgtype = PAGE_BUDDYALLOCATOR;
		memset(&(pgfm->meta_other), 0, sizeof(pgfm->meta_other));
		unlock(&(pgfm->lk));
		bkp_free(pgfm);
		return ret;
	}
	return 0;
}

int     vm_mmap_lazy(vma_t* vma, viraddr_t va, int order, vm_prot prot)
{
	return -EFAULT;
}

int     vm_unmap(vma_t* vma, viraddr_t va, int order)
{
	if(vma == NULL || !VAL_IS_PAGE_ALIGNED(va)\
		 	|| (vma->kernel_vma != KVMA)\
			|| !(order >=0 && order < MAX_ORDER))
		return -EFAULT;
	return __vm_unmap(vma, va, (1<<order)*PAGE_SIZE, VMA_MEMORY, 1);
}

vma_t*  vma_dup(vma_t* vma_copied)
{
	if(vma_copied == NULL\
		|| vma_copied->kernel_vma != kern_vma\
		|| vma_copied->hw_pgtable == NULL)
		return NULL;
	vma_t* ret = kmem_obj_alloc(vma_cache);
        if(ret == NULL)
                return ret;
        ASSERT(ret->hw_pgtable == 0 && ret->kernel_vma == 0);
        ret->hw_pgtable = (pgt*)bk_alloc_zero(0);
        if(ret->hw_pgtable == NULL)
                goto free_vma_cache;
        rwlock_init(&(ret->rwlk));
        ret->kernel_vma = kern_vma;
        avl_root_init(&(ret->a_root));
        INIT_LIST_HEAD(&(ret->l_list));
	r_lock(&(vma_copied->rwlk));
	struct list_head* iterator;
	list_for_each(iterator, &(vma_copied->l_list))
	{
		vma_area_t* vma_area = list_entry(iterator, vma_area_t, l_node);
		if(vma_area->vmar_typ == VMA_IO || vma_area->vmar_typ == VMA_KERNEL)
		{
			ASSERT(VAL_IS_PAGE_ALIGNED(vma_area->va) \
					&& VAL_IS_PAGE_ALIGNED(vma_area->pa)\
					&& VAL_IS_PAGE_ALIGNED(vma_area->size)\
					&& vma_area->size > 0\
					&& vma_area->pgfm == NULL);
			if(__vm_mmap(ret, vma_area->va, vma_area->pa, vma_area->prot,\
						vma_area->size, vma_area->vmar_typ, 1) != 0)
			{
				r_unlock(&(vma_copied->rwlk));
				goto free_map_area;
			}
		}
		else
		{
			ASSERT(vma_area->vmar_typ == VMA_MEMORY);
			ASSERT(VAL_IS_PAGE_ALIGNED(vma_area->va) \
					&& VAL_IS_PAGE_ALIGNED(vma_area->size)\
					&& vma_area->size >0);
			if(vma_area->pa == (phyaddr_t)-1)
			{
				ASSERT(vma_area->pgfm == NULL);
				if(__vm_mmap(ret, vma_area->va, vma_area->pa, vma_area->prot,\
							vma_area->size, vma_area->vmar_typ, 1)!= 0)
				{
					r_unlock(&(vma_copied->rwlk));
					goto free_map_area;
				}

			}
			else
			{
				ASSERT(vma_area->pgfm != NULL \
						&& vma_area->pa != (phyaddr_t)-1);
				if((vma_area->prot & PROT_W) != 0)
				{
					int want_order = PGNUM2ORDER((vma_area->size)/PAGE_SIZE);
					ASSERT(want_order >= 0);
					pageframe_t* pgfm = bkp_alloc_zero(want_order);
					if(pgfm == NULL)
					{
						r_unlock(&(vma_copied->rwlk));
						goto free_map_area;
					}
					lock(&(pgfm->lk));
					ASSERT((pgfm->meta).buddy_order == want_order\
							&& (pgfm->meta).buddy_flags == PG_BUDDY_FLAG_HEAD\
							&& pgfm->pgtype == PAGE_BUDDYALLOCATOR);
					pgfm->pgtype = VMA_AREA;
					(pgfm->meta_other).ref_count = 0;
					(pgfm->meta_other).vma_flags = 0;
					unlock(&(pgfm->lk));
					phyaddr_t pa = pg2pa(pgfm);
					if(__vm_mmap(ret, vma_area->va, pa, vma_area->prot,\
								vma_area->size, vma_area->vmar_typ, 1) != 0)
					{
						lock(&(pgfm->lk));
						pgfm->pgtype = PAGE_BUDDYALLOCATOR;
						unlock(&(pgfm->lk));
						bkp_free(pgfm);
						r_unlock(&(vma_copied->rwlk));
						goto free_map_area;
					}
				}
				else
				{
					if(__vm_mmap(ret, vma_area->va, vma_area->pa, vma_area->prot,\
								vma_area->size, vma_area->vmar_typ, 1) != 0)
					{
						r_unlock(&(vma_copied->rwlk));
						goto free_map_area;
					}
				}
			}
		}

	}
	r_unlock(&(vma_copied->rwlk));
        return ret;
free_map_area:
        vma_free(ret);
        return NULL;
free_vma_cache:
        kmem_obj_free(vma_cache, ret);
        return NULL;
}

vma_t*  vma_dup_cow(vma_t* vma_copied)
{
	return NULL;
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
