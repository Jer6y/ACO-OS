#ifndef __ACO_ASM_RISCV_FDT_H
#define __ACO_ASM_RISCV_FDT_H

#include <aco/linkage.h>

static inline void* __arch_dtb_get(void)
{
	return (void*)(get_rv_dtb());
}


#endif /* __ACO_ASM_RISCV_FDT_H */
