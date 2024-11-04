#include <mm/vm.h>
#include <mm/buddy.h>
#include <aco/assert.h>
#include <aco/errno.h>

#define VMPROT2RVPROT(x) 	(x)

static inline int vm_walkthrough(pgt* pagetable, viraddr_t va, int index, phyaddr_t pa, vm_prot flags, int* pages)
{
        ASSERT(index >=0 && index < 3);
	ASSERT(pages != NULL);
        int idx = SV39_Index2Id(va,index);
        ASSERT(idx >=0 && idx < PTE_NUMS);
	int ret =0;
	for(;idx<PTE_NUMS;idx++)
	{
		if(index == 2)
		{
			if(!PTE_VALID((pagetable->ptes)[idx]))
			{
				(pagetable->ptes)[idx] = SV39_Pa2PTE(pa, flags);
				(*pages)--;
				va += PAGE_SIZE;
				pa += PAGE_SIZE;
				if(*pages == 0)
				{
					ret = 0;
					break;
				}
			}
			else
			{
				ret = -EEXIST;
				break;
			}
		}
		else
		{
			if(PTE_VALID((pagetable->ptes)[idx]))
			{
				if(!PTE_NEXTPGT((pagetable->ptes)[idx]))
				{
					ret = -EFAULT;
					break;
				}
				phyaddr_t pa_next_pagetable = (phyaddr_t)SV39_PTE2Pa((pagetable->ptes)[idx]);
                        	pgt*      next_pagetable = (pgt*)pa2va(pa_next_pagetable);
                        	ASSERT(PA_PAGE_ALIGNED(pa_next_pagetable) && VA_PAGE_ALIGNED(next_pagetable));
                        	int cur_pages = *pages;
				ret = vm_walkthrough(next_pagetable, va, index+1, pa, flags, pages);
				va += (cur_pages - (*pages))*PAGE_SIZE;
				pa += (cur_pages - (*pages))*PAGE_SIZE;
				if(ret)
					break;
				if(*pages == 0)
				{
					ret = 0;
					break;
				}
			}
			else
			{
				uintptr_t ptr = (uintptr_t)(bk_alloc_zero(0));
				if(ptr ==0)
				{
					ret = -ENOMEM;
					break;
				}
				uintptr_t pa_ptr = (uintptr_t)va2pa(ptr);
                                ASSERT(PA_PAGE_ALIGNED(pa_ptr) && VA_PAGE_ALIGNED(ptr));
				(pagetable->ptes)[idx] = SV39_Pa2PTE(pa_ptr, VM_PROT_V);
				int cur_pages = *pages;
				ret = vm_walkthrough((pgt*)ptr, va, index+1, pa, flags, pages);
				va += (cur_pages - (*pages))*PAGE_SIZE;
				pa += (cur_pages - (*pages))*PAGE_SIZE;
				if(ret)
					break;
				if(*pages == 0)
				{
					ret = 0;
					break;
				}
			}
        	}
	}
        return ret;
}

static inline void riscv_unmap(pgt* pgtable, viraddr_t va, int index, int* pages)
{
	ASSERT(index >=0 && index < 3);
	ASSERT(pages != NULL);
	int idx = SV39_Index2Id(va, index);
	ASSERT(idx >=0 && idx < PTE_NUMS);
	for(;idx < PTE_NUMS;idx++)
	{
		if(index != 2)
		{
			if(PTE_NEXTPGT((pgtable->ptes)[idx]))
			{
				phyaddr_t pa_next_pgtable = (phyaddr_t)SV39_PTE2Pa((pgtable->ptes)[idx]);
				pgt* next_pgtable = (pgt*)pa2va(pa_next_pgtable);
				ASSERT(next_pgtable != NULL && PA_PAGE_ALIGNED(pa_next_pgtable) && VA_PAGE_ALIGNED(next_pgtable));
				int cur_pages = (*pages);
				riscv_unmap(next_pgtable, va, index+1, pages);
				va  += (cur_pages - (*pages))*PAGE_SIZE;
				if((cur_pages - (*pages)) == SV39_INDEX_PAGES(index))
				{
					(pgtable->ptes)[idx] = 0;
					bk_free((void*)next_pgtable);
				}
				if(*pages == 0)
					break;
			}
			else
			{
				int sv39_index_max_pages = SV39_INDEX_PAGES(index);
				if((*pages) > sv39_index_max_pages)
				{
					*pages -= sv39_index_max_pages;
					va  += sv39_index_max_pages*PAGE_SIZE;
				}
				else
				{
					*pages = 0;
					va += (sv39_index_max_pages - (*pages))*PAGE_SIZE;
					break;
				}
			}
		}
		else
		{
			(pgtable->ptes)[idx] = 0;
			(*pages)--;
		 	// va += PAGE_SIZE;
			if(*pages == 0)
				break;
		}
	}
	return;
}

static inline void riscv_free_pgtable(pgt* pgtable, int index)
{
	ASSERT(index >=0 && index < 3);
	for(int i=0;i<PTE_NUMS;i++)
	{
		if(index == 2)
		{
			if(PTE_VALID((pgtable->ptes)[i]))
				(pgtable->ptes)[i] = 0;
		}
		else
		{	
			if(PTE_NEXTPGT((pgtable->ptes)[i]))
			{
				phyaddr_t pa_next_pgtable = (phyaddr_t)SV39_PTE2Pa((pgtable->ptes)[i]);
				pgt* next_pgtable = (pgt*)pa2va(pa_next_pgtable);
				ASSERT(PA_PAGE_ALIGNED(pa_next_pgtable) && VA_PAGE_ALIGNED(next_pgtable));
				riscv_free_pgtable(next_pgtable, index+1);
				(pgtable->ptes)[i] = 0;
				bk_free((void*)next_pgtable);
			}
			else
				ASSERT(!PTE_VALID((pgtable->ptes)[i]));
		}
	}
	return;
}


int __arch_hw_mmap(pgt* pgtable,viraddr_t va, phyaddr_t pa, vm_prot prot, uintptr_t size)
{
	ASSERT(pgtable != NULL && PA_PAGE_ALIGNED(pgtable));
	ASSERT(VA_PAGE_ALIGNED(va) && PA_PAGE_ALIGNED(pa));
	ASSERT(VAL_IS_PAGE_ALIGNED(size));
	if(size ==0)
		return 0;
	int npages = size / PAGE_SIZE;
	ASSERT(npages >= 0);
	int cur_pages = npages;
        int ret = vm_walkthrough(pgtable, va, 0, pa, VMPROT2RVPROT(prot), &npages);
        if(ret)
	{
		__arch_hw_unmap(pgtable, va, (cur_pages - npages)*PAGE_SIZE);
		return ret;
	}
	return 0;
}

void    __arch_hw_unmap(pgt* pgtable,viraddr_t va, uintptr_t size)
{
	ASSERT(pgtable != NULL \
			&& VAL_IS_PAGE_ALIGNED(size)\
			&& VA_PAGE_ALIGNED(va));
	if(size ==0)
		return ;
	int npages = size/PAGE_SIZE;
	ASSERT(npages >= 0);
	riscv_unmap(pgtable, va, 0, &npages);
	ASSERT(npages == 0);
}

void    __arch_hw_unmap_all(pgt* pgtable)
{
	ASSERT(pgtable != NULL);
	riscv_free_pgtable(pgtable, 0);
}

void    __arch_hw_reset_prot(pgt* pgtable,vm_prot prot,viraddr_t va, uintptr_t size)
{
	ASSERT(pgtable != NULL\
			&& VAL_IS_PAGE_ALIGNED(size)\
			&& VA_PAGE_ALIGNED(va));
	if(size ==0)
		return;
	return;
}

#include <csr.h>
pgt*    __arch_hw_get_current_pgtable()
{
	uintptr_t satp = (uintptr_t)r_satp();
	phyaddr_t pg_pa = SV39_SATP2PGT(satp);
	ASSERT(PA_PAGE_ALIGNED(pg_pa));
	return (pgt*)pa2va(pg_pa);
}

void    __arch_hw_load_pgtable(pgt* pgtable)
{
	ASSERT(pgtable != NULL && PA_PAGE_ALIGNED(pgtable));
	phyaddr_t pg_pa = va2pa(pgtable);
	uintptr_t satp = SV39_PGT2SATP(pg_pa);
	asm volatile("csrw satp, %0"::"r"(satp):"memory");
	asm volatile("sfence.vma zero, zero");
}

