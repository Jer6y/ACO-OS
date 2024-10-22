#include <aco/types.h>
#include <aco/linkage.h>
#include <mm/page.h>
#include <mm/early_mm.h>
#include <aco/errno.h>
#include <aco/printf.h>

typedef	uint64_t pte;

#define	ENTRY_NUM (PAGE_SIZE/sizeof(pte))
struct pgt {
	pte ptes[ENTRY_NUM];
} PACKED;

typedef struct pgt pgt;

#define VM_PROT_V	(1ull<<0)
#define VM_PROT_R	(1ull<<1)
#define VM_PROT_W	(1ull<<2)
#define VM_PROT_X	(1ull<<3)
#define VM_PROT_U	(1ull<<4)
#define VM_PROT_G	(1ull<<5)
#define VM_PROT_A	(1ull<<6)
#define VM_PROT_D	(1ull<<7)

#define PTE_VALID(pte)		((pte) & VM_PROT_V)
#define PTE_WRITEABLE(pte)	((pte) & VM_PROT_W)
#define PTE_READABLE(pte)	((pte) & VM_PROT_R)
#define PTE_EXCUTABLE(pte)	((pte) & VM_PROT_X)
#define PTE_NEXTPGT(pte)	(PTE_VALID(pte) && (!PTE_WRITEABLE(pte) && !PTE_READABLE(pte) && !PTE_EXCUTABLE(pte)))

typedef uint16_t  prot_t;

typedef uintptr_t viraddr_t;

typedef uintptr_t phyaddr_t;


#define SV39_Index2Shift(index)		(12 + 9 * (2-(index)))
#define SV39_Index2Bit(index)		((0x1ffull << SV39_Index2Shift(index)))
#define SV39_Index2Id(va,index) 	(((va) & SV39_Index2Bit(index)) >> SV39_Index2Shift(index))

#define SV39_Pa2PTE(pa, prot)		(((((pa) >> PAGE_OFFSET) & 0xfffffffffffull) << 10) | ((prot) & 0x3ffull) )
#define SV39_PTE2Pa(pte)		(((((pte) & (~0x3ffull)) >> 10) & 0xfffffffffffull)<< 12)

FUNC_BUILTIN int vm_walkthrough(pgt* pagetable, viraddr_t va, int index, phyaddr_t pa, prot_t flags)
{
	if(pagetable == NULL || !PA_PAGE_ALIGNED((uintptr_t)pagetable))
		return -EINVAL;
	if(!PA_PAGE_ALIGNED(va) || !PA_PAGE_ALIGNED(pa))
		return -EINVAL;
	if(!(index >=0 && index <3))
		return -EINVAL;
	int idx = SV39_Index2Id(va,index);
	if(!(idx >=0 && idx <512))
		return -EFAULT;
	if(!PTE_VALID((pagetable->ptes)[idx]))
	{
		if(index == 2)
		{
			(pagetable->ptes)[idx] = SV39_Pa2PTE(pa, flags);
		}
		else
		{
			uintptr_t ptr = (uintptr_t)mem_alloc();
			if(ptr == 0)
				return -ENOMEM;
			(pagetable->ptes)[idx] = SV39_Pa2PTE(ptr, VM_PROT_V);
			vm_walkthrough((pgt*)ptr, va, index+1, pa, flags);
		}
	}
	else
	{
		if(index == 2)
			return -EEXIST;
		if(!PTE_NEXTPGT((pagetable->ptes)[idx]))
			return -EFAULT;

		pgt* next_pagetable = (pgt*)SV39_PTE2Pa((pagetable->ptes)[idx]);
		int ret = vm_walkthrough(next_pagetable, va, index+1, pa, flags);
		if(ret)
			return ret;
	}
	return 0;
}

FUNC_BUILTIN int vm_mmap(pgt** ptr_pagetable, viraddr_t va, phyaddr_t pa, size_t size, prot_t flags)
{
	if((va & PAGE_ALIGN_BIT) != (pa & PAGE_ALIGN_BIT))
		return -EINVAL;
	if(size % PAGE_SIZE != 0)
		return -EINVAL;
	if(ptr_pagetable == NULL)
		return -EINVAL;
	if((*ptr_pagetable) == NULL)
	{
		(*ptr_pagetable) = mem_alloc();
		if((*ptr_pagetable) ==NULL)
			return -ENOMEM;
	}
	va = PAGE_DOWNS_ALIGN(va);
	pa = PAGE_DOWNS_ALIGN(pa);
	for(;size >0;size -= PAGE_SIZE)
	{
		int ret = vm_walkthrough((*ptr_pagetable), va, 0, pa, flags);
		if(ret)
			return ret;
		va += PAGE_SIZE;
		pa += PAGE_SIZE;
	}
	return 0;
}

#define SATP_MODE_SV39		(8ull << 60)
#define SATP_MODE_SV48		(9ull << 60)
#define SV39_PGT2SATP(pgt) 	((((uintptr_t)(pgt) & 0xfffffffffffull) >> 12) |  SATP_MODE_SV39)

uintptr_t setup_earlyvm(uintptr_t pa_dtb, uintptr_t core_id)
{
	int ret;
	char* kernel_start  = get_kernel_start();
	char* kernel_end    = get_kernel_end();
	char* seperate_line = get_seperate_line();
	size_t bef_size = (size_t)(seperate_line - kernel_start);
	size_t aft_size = (size_t)(kernel_end - seperate_line);
	ASSERT(bef_size % PAGE_SIZE ==0);
	aft_size = PAGE_UPPER_ALIGN(aft_size);
	pgt* pagetable = NULL;
	ret = vm_mmap(&pagetable, (viraddr_t)KERNEL_OFFSET, (phyaddr_t)kernel_start, bef_size, VM_PROT_V | VM_PROT_R | VM_PROT_X);
	ASSERT(ret ==0);
	ret = vm_mmap(&pagetable, (viraddr_t)KERNEL_OFFSET + bef_size, (phyaddr_t)seperate_line, aft_size, VM_PROT_V | VM_PROT_R | VM_PROT_W);
	ASSERT(ret ==0);
	//early_mm refresh
	refresh_for_mmu();
	return SV39_PGT2SATP(pagetable);
}
