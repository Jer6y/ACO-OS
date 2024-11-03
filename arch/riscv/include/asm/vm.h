#ifndef __ACO_ASM_VM_H
#define __ACO_ASM_VM_H

#include <aco/bits.h>
#include <generated/autoconf.h>

#if ( BITS64 == 1)


#define VM_PROT_V       (1ull<<0)
#define VM_PROT_R       (1ull<<1)
#define VM_PROT_W       (1ull<<2)
#define VM_PROT_X       (1ull<<3)
#define VM_PROT_U       (1ull<<4)
#define VM_PROT_G       (1ull<<5)
#define VM_PROT_A       (1ull<<6)
#define VM_PROT_D       (1ull<<7)

#define PTE_VALID(pte)          ((pte) & VM_PROT_V)
#define PTE_WRITEABLE(pte)      ((pte) & VM_PROT_W)
#define PTE_READABLE(pte)       ((pte) & VM_PROT_R)
#define PTE_EXCUTABLE(pte)      ((pte) & VM_PROT_X)
#define PTE_NEXTPGT(pte)        (PTE_VALID(pte) && (!PTE_WRITEABLE(pte) && !PTE_READABLE(pte) && !PTE_EXCUTABLE(pte)))

#define SV39_Index2Shift(index)         (12 + 9 * (2-(index)))
#define SV39_Index2Bit(index)           ((0x1ffull << SV39_Index2Shift(index)))
#define SV39_Index2Id(va,index)         (((va) & SV39_Index2Bit(index)) >> SV39_Index2Shift(index))

#define SV39_Pa2PTE(pa, prot)           (((((pa) >> PAGE_OFFSET) & 0xfffffffffffull) << 10) | ((prot) & 0x3ffull) )
#define SV39_PTE2Pa(pte)                (((((pte) & (~0x3ffull)) >> 10) & 0xfffffffffffull)<< 12)

#define SATP_MODE_SV39          (8ull << 60)
#define SATP_MODE_SV48          (9ull << 60)
#define SV39_PGT2SATP(pgt)      ((((uintptr_t)(pgt) & 0xfffffffffffull) >> 12) |  SATP_MODE_SV39)
#define SV39_SATP2PGT(satp)	(((uintptr_t)(satp) & 0xfffffffffffull) << 12)

typedef unsigned long long __arch_pte;

#define PTE_NUMS	(1<<CONFIG_PAGE_OFFSET)/sizeof(__arch_pte)
typedef struct __arch_pgt {
	__arch_pte  ptes[PTE_NUMS];
} __arch_pgt;

static inline int SV39_INDEX_PAGES(int index)
{
        if(index == 0)
                return PTE_NUMS*PTE_NUMS;
        else if(index == 1)
                return PTE_NUMS;
        else if(index == 2)
                return 1;
	return 0;
}

#else
  #error "unsupport riscv32"
#endif

#endif /* __ACO_ASM_VM_H */
