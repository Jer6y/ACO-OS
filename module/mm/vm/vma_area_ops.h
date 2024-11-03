#ifndef __ACO_MODULE_VM_VMAREA_OPS_H
#define __ACO_MODULE_VM_VMAREA_OPS_H

#include <mm/vm.h>


void vma_area_val_init(vma_area_t* vma_area, vmar_type_t vmar_tp, \
		       viraddr_t va, phyaddr_t pa, uintptr_t size, \
		       pageframe_t* pgfm, vm_prot prot);

void vma_area_val_deinit(vma_area_t* vma_area);

/*
 * Desc : hange "vma_area" node to "vma->l_list" and "vma->a_root" 
 *
 * Param:
 * 	    vma 	: the vma want to be hanged
 * 	    vma_area	: the area will hang
 * Retrn:
 * 	    0 		: hang successfully
 * 	   <0		: errno
 * 	   -EEXSIT	: vma_area conflit with other vma_area in "vma"
 * Note :
 * 	    this operation is not locked
 * 	    if you want to thread-safety
 * 	    do lock before call this
 */
int vma_area_hang(vma_t* vma, vma_area_t* vma_area);

/*
 * Desc : unhang "vma_area" node in the "vma"
 * 
 * Param:
 * 	    vma 	: the vma have the vma_area
 * 	    vma_area	: the area which will be unhanged from the vma
 *
 * Note :
 * 	    this operation is not locked
 * 	    if you want to thread-safety
 * 	    do lock before call this
 */
void vma_area_unhang(vma_t* vma, vma_area_t* vma_area);


#endif /* __ACO_MODULE_VM_VMAREA_OPS_H */
