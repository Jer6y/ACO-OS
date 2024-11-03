#include <mm/vm.h>
#include <aco/init.h>
#include <mm/slab.h>
#include <aco/linkage.h>
#include <aco/assert.h>
#include <aco/printf.h>
#include <aco/errno.h>
#include <aco/log.h>

#include "vm_internal.h"

int init_vma_num;
int init_vma_area_num;

kmem_cache_t* vma_cache;
kmem_cache_t* vma_area_cache;

vma_t*  kern_vma;

DEFINE_ATOMIC(vm_module_init_done,0);

FUNC_BUILTIN int vm_module_init(void)
{
	if(atomic_fetch_and_add(&vm_module_init_done,0) != 0)
		return 0;
        int ret;
        vma_cache = kmem_cache_alloc();
        if(vma_cache == NULL)
        {
                ret = -ENOBUFS;
                return ret;
        }
        vma_area_cache = kmem_cache_alloc();
        if(vma_area_cache == NULL)
        {
                ret = -ENOBUFS;
                goto free_vma_cache;
        }
        ret = kmem_cache_init(vma_cache, sizeof(vma_t), 0, 3000, 3);
        if(ret < 0)
                goto free_vma_area_cache;
        ret = kmem_cache_init(vma_area_cache, sizeof(vma_area_t), 0, 3000, 30);
        if(ret < 0)
                goto vma_cache_deinit;
        kern_vma = kmem_obj_alloc(vma_cache);
        if(kern_vma == NULL)
        {
                ret = -ENOBUFS;
                goto vma_area_cache_deinit;
        }
        rwlock_init(&(kern_vma->rwlk));
        kern_vma->kernel_vma = kern_vma;
        kern_vma->hw_pgtable = hw_get_current_pgtable();
        avl_root_init(&(kern_vma->a_root));
        INIT_LIST_HEAD(&(kern_vma->l_list));
        uintptr_t seperate_line = (uintptr_t)get_seperate_line();
        ASSERT(VAL_IS_PAGE_ALIGNED(seperate_line));
        ASSERT((seperate_line - PGFRAME_VA_START) > 0);
        uintptr_t seperate_size = seperate_line - PGFRAME_VA_START;
        ret = __vm_mmap(kern_vma, PGFRAME_VA_START, PGFRAME_PA_START, PROT_V | PROT_X | PROT_R, seperate_size, VMA_KERNEL, 0);
        if(ret < 0)
                goto free_kernel_vma;
        ret = __vm_mmap(kern_vma, PGFRAME_VA_START + seperate_size, PGFRAME_PA_START + seperate_size, \
                                PROT_V | PROT_W | PROT_R, PGFRAME_SIZE - seperate_size, VMA_KERNEL, 0);
        if(ret <0)
                goto free_kernel_vma;
        init_vma_num = vma_cache->obj_num;
        init_vma_area_num = vma_area_cache->obj_num;
        vm_dump_information(kern_vma, printf);
        log_debug("[VMA]: vma objnum : %d", vma_cache->obj_num);
        log_debug("[VMA]: vma objsiz : %d", vma_cache->obj_size);
        log_debug("[VMA]: vmar objnm : %d", vma_area_cache->obj_num);
        log_debug("[VMA]: vmar objsz : %d", vma_area_cache->obj_size);
        log_debug("[VMA]: init vma/vma_area cache done!");
        ret = vm_check(kern_vma);
        if(ret)
                log_debug("[VMA]: test for kern_vma area 0/1");
        else
                log_debug("[VMA]: test for kern_vma area 1/1");
	
	atomic_fetch_and_add(&vm_module_init_done,1);
        return ret;
free_kernel_vma:
        vma_free(kern_vma);
vma_area_cache_deinit:
        kmem_cache_deinit(vma_area_cache);
vma_cache_deinit:
        kmem_cache_deinit(vma_cache);
free_vma_area_cache:
        kmem_cache_free(vma_area_cache);
free_vma_cache:
        kmem_cache_free(vma_cache);
        return ret;
}

FUNC_BUILTIN void vm_module_exit(void)
{
        return;
}

REGISTER_MODULE_INIT(PRIO_4, vm_module_init);
REGISTER_MODULE_EXIT(PRIO_4, vm_module_exit);
