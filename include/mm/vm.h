#ifndef __ACO_VMM_H
#define __ACO_VMM_H

#include <aco/types.h>
#include <aco/list.h>
#include <aco/avlmini.h>
#include <aco/spinlock.h>
#include <mm/pageframe.h>
#include <aco/rwlock.h>

//arch porting implementation
#include <asm/vm.h>

#define PROT_V          (1<<0)
#define PROT_R          (1<<1)
#define PROT_W          (1<<2)
#define PROT_X          (1<<3)
#define PROT_U          (1<<4)
#define PROT_G          (1<<5)
#define PROT_A          (1<<6)
#define PROT_D          (1<<7)
typedef uint16_t        vm_prot;

/*
 * Desc : pagetable entry
 * Note : Need arch porting implementation for structer
 * struct __arch_pte;
 */
typedef __arch_pte 	pte;

/*
 * Desc : pagetable 
 * Note : Need arch porting implementation for structer
 * struct __arch_pgt;
 */
typedef __arch_pgt	pgt;


int 	__arch_hw_mmap(pgt*,viraddr_t,phyaddr_t,vm_prot,uintptr_t);

void 	__arch_hw_unmap(pgt*,viraddr_t,uintptr_t);

void 	__arch_hw_unmap_all(pgt*);

void 	__arch_hw_reset_prot(pgt*,vm_prot,viraddr_t,uintptr_t);

pgt* 	__arch_hw_get_current_pgtable();

void 	__arch_hw_load_pgtable(pgt*);

/*
 * Desc : mmap "va" to "pa", set the va for 
 * 	  "prot", map "size" length in "pgtable"
 * Param:
 * 	    pgtable : hwpagetable need to be operated
 * 	    va	    : virtual address want to map
 * 	    pa      : physical address want to map to
 * 	    prot    : Permission in the pagetable for "va"
 * 	    size    : length want to map
 * Retrn:
 * 	    0  	    : success
 * 	   <0	    : errno
 * Note :   Need arch porting implementation function
 *   	    int __arch_hw_mmap(pgt*,viraddr_t,phyaddr_t,vm_prot,size_t);
 */
static inline 	int	hw_mmap(pgt* pgtable, viraddr_t va, phyaddr_t pa, vm_prot prot,\
				uintptr_t size)
{
	return __arch_hw_mmap(pgtable, va, pa, prot, size);
}

/*
 * Desc : unmmap , unmap "size" length in "pgtable"
 * Param:
 *          pgtable : hwpagetable need to be operated
 *          va      : virtual address want to unmap
 *          size    : length want to unmap
 * Retrn:
 *          0       : success
 *         <0       : errno
 * Note :   Need arch porting implementation function
 *          void __arch_hw_unmap(pgt*,viraddr_t,size_t);
 */
static inline	void	hw_unmap(pgt* pgtable, viraddr_t va, uintptr_t size)
{
	__arch_hw_unmap(pgtable, va, size);
}

/*
 * Desc : unmap all fot "pgtable"
 * Param:
 * 	    pgtable : hwpagetable need to be operated
 * Note : Need arch poring implementation function
 *        void __arch_hw_unmap_all(pgt*);
 */
static inline	void	hw_unmap_all(pgt* pgtable)
{
	__arch_hw_unmap_all(pgtable);
}

/*
 * Desc : reset map address permission for "pgtable"
 * Param:
 * 	    pgtable : hwpagetable need to be operated
 * 	    va      : address want to reset
 * 	    prot    : permission want to reset
 * 	    size    : size for the reset memory segment
 * Note : Need arch porting implementation function
 * 	  void __arch_hw_reset_prot(pgt*,vm_prot,viraddr_t,size_t)
 */
static inline	void	hw_reset_prot(pgt* pgtable, vm_prot prot, viraddr_t va, uintptr_t size)
{
	__arch_hw_reset_prot(pgtable, prot, va, size);
}

/*
 * Desc : get current core's hardware pagetable
 * Param:
 *
 * Retun:
 * 	 	pgt* : the current pagetable to return
 * Note : Need arch porting implementation function
 * 	  pgt* __arch_hw_get_current_pgtable();
 */
static inline	pgt*	hw_get_current_pgtable()
{
	return __arch_hw_get_current_pgtable();
}

/*
 * Desc : load "pgtable"
 * Param:
 * 	    pgtable : the pagetable need to be loaded
 * Note : Need arch porting implementation function
 *  	  void __arch_hw_load_pgtable(pgt*);
 */
static inline	void	hw_load_pgtable(pgt* pgtable)
{
	__arch_hw_load_pgtable(pgtable);
}

// becareful! , all those hw_xxx function is private
// it is can not be used by user or other module
// these function have very strict paramter assertion
// all pgtable should be not NULL and va,pa,size shoule be page-aligned
// never use them!!!


typedef enum vma_area_type {
	VMA_KERNEL	= 0x0u		,
	VMA_IO				,
	VMA_MEMORY			,
	VMA_NOREACH			,
} vmar_type_t;

typedef struct vma_area {
	vmar_type_t		vmar_typ; // this flag is to determin if we need to do bkp_free 
					  // for a area for KERNEL or IO , just unmap it
					  // for a area for MEMORY , means it is allocated by buddy
					  // we should do the software unmap when unmap the area
	viraddr_t		va;
	uintptr_t		size;
	phyaddr_t		pa;
	pageframe_t*		pgfm;	
	vm_prot			prot;
	struct list_head	l_node; // list node to be hanged on the vma_t , for iterate
	struct avl_node		a_node;	// avl  node to be hanged on the vma_t , for search
} vma_area_t;

typedef struct vma {
	rwlock_t 		rwlk;
	pgt*			hw_pgtable;
	struct vma* 		kernel_vma;  //every vma should have a kernel_vma area
	struct avl_root		a_root;
	struct list_head	l_list; 
} vma_t;

extern vma_t* kern_vma;
#define KVMA  kern_vma

extern int    init_vma_num;
extern int    init_vma_area_num;
#define INIT_VMA_NUM		init_vma_num
#define INIT_VMA_AREA_NUM	init_vma_area_num


/*
 * Desc : get (vma_t*) object num
 * Retrn: 
 *        int : object number of cached (vma_t*)
 */
int 	vma_cache_number();

/*
 * Desc : get (vma_area_t*) object num
 * Retrn:
 *        int : object number of cached (vma_area_t*)
 * Note : this often not used by user
 *        it will used by testcase to test if the
 *        slab allocator works well
 */
int	vma_area_cache_number();

/*
 * Desc : get a vma object initialized
 * Retrn:
 * 	   vma_t* : NULL if error
 * 	            other if success
 * Note : get the vma_t* will have the kernel 
 *        memory mapped, don't worry about
 *        if you need to map kernel address
 *        to it
 */
vma_t* 	vma_alloc();

vma_t*	vma_dup(vma_t* vma_copied);

vma_t*  vma_dup_cow(vma_t* vma_copied);
/*
 * Desc : free a vma object
 * Param:
 * 	    vma: the vma want to free
 * Note : don't worry about if vma have some pages
 *        mapped, it will automatically free these
 *        pages
 */
void   	vma_free(vma_t* vma); //do vma_deinit before, otherwise system will carsh or leak memory

/*
 * Desc : map a io addres in to vma
 * Param:
 * 	    vma : the vma want to use
 * 	    va  : the virtual address in the 'vma'
 * 	    pa  : the physical address want to map to
 * 	    prot: permission of the addres
 * 	    size: size want to map
 * Retrn:
 * 	    0   : success
 * 	   <0   : errno
 * 	 -EFAULT: "vma" is NULL or va/pa is not page-aligned 
 * 	 -EEXIST: in "vma" have a area conflict with new area
 * 	-ENOBUFS: no more memory to do this thing
 * Note : va and pa must be page-aligned!!!!!!!!!!
 *        remember to set RPOT_V to prot
 *        to validate the addres
 */
int  	vm_iomap(vma_t* vma, viraddr_t va, phyaddr_t pa, vm_prot prot, uintptr_t size);

/*
 * Desc : unmap the io address in the vma
 * Param:
 * 	    vma : the vma want to use
 * 	    va  : the addres want to unmap
 * 	    size: the size want to unmap
 * Note : va must be page-aligned
 *        if you meet such situation
 *         _____
 *        |_____| va  : 0x1000 -> pa : 0x0
 *        |_____| size: 0x1000  
 *        |_____| prot: rwx
 *        |_____|
 *         _____
 *        |_____| va  : 0x2000 -> pa : 0x1000
 *        |_____| size: 0x2000
 *        |_____| prot: rwx
 *        |_____|
 *        you use "vm_iomap" twice to setup it
 *        {
 *            vm_iomap(vma, 0x1000, 0x0, PROT_V | PROT_R | PROT_W | PROT_X, 0x1000);
 *	      vm_iomap(vma, 0x2000, 0x1000, PROT_V | PROT_R | PROT_W | PROT_X, 0x2000);
 *        }
 *        can you use vm_iounmap(vma, 0x1000, 0x3000) to unmap all vma_area in the vma?
 *        answer is no! if you use vm_iomap twice , you must use vm_iounmap twice
 *        nerver think we will merge this two region to one region to be managed
 *        it costs much. and we often use a <vm_iomap / vm_iounmap> as a pair
 *        in our code. It is not necessary to take the costs to merge two region
 *        in one region
 * Retrn: 
 * 	 	0  : unmap succes
 * 	       <0  : errno
 */
int     vm_iounmap(vma_t* vma, viraddr_t va, uintptr_t size);

/*
 * Desc : map a free memory to "va" in "vma" , size is (1<<order)*PAGESIZE
 *        prot is "prot"
 * Param:
 * 	    vma 	: the vma want to map
 * 	    va		: the virtual address in "vma" want to map
 * 	    order	: (1<<order)*PAGE_SIZE is mapped
 * 	    prot	: permission for the new map area
 * Retrn:
 * 	    0		: success
 * 	   <0		: errno
 * 	   -EFAULT	: "vma" is NULL or va is not page-aligned
 * 	   -ENOMEM	: no memory to map
 * Note : this function is different to "vm_iomap" , "vm_iomap"
 *  	  is to setup a "va" to a "pa" for io, 
 *  	  but vm_mmap will setup "va" to a new free memory
 *  	  but not io, this new free memory is managed by buddy_system
 */
int 	vm_mmap(vma_t* vma, viraddr_t va, int order, vm_prot prot);

int 	vm_mmap_lazy(vma_t* vma, viraddr_t va, int order, vm_prot prot);

int 	vm_unmap(vma_t* vma, viraddr_t va, int order);


int 	vm_check(vma_t* vma);
typedef int (*output_callback)(const char* str,...);
void 	vm_dump_information(vma_t* vma, output_callback out_cb);


#endif /* __ACO_VMM_H */
