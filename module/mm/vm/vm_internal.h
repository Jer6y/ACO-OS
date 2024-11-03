#ifndef __VM_MODULE_INTERNEL_H
#define __VM_MODULE_INTERNEL_H

#include <mm/slab.h>

extern kmem_cache_t* vma_cache;
extern kmem_cache_t* vma_area_cache;

int	__vm_mmap(vma_t* vma, viraddr_t va, phyaddr_t pa, vm_prot prot, uintptr_t size, vmar_type_t type, bool need_hwmap);
int 	__vm_unmap(vma_t* vma, viraddr_t va, uintptr_t size, vmar_type_t type, int need_hwunmap);

#endif /* __VM_MODULE_INTERNEL_H */
