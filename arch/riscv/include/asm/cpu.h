#ifndef __ACO_ASM_CPU_H
#define __ACO_ASM_CPU_H

#include <csr.h>
#include <generated/autoconf.h>

#ifdef CONFIG_QEMU_RV64
#define __arch_cpu_num 4
#endif 

static inline int __arch_cpu_get()
{
	return (int)(r_tp());
}

static inline void __arch_cpu_intr_off()
{
	intr_off();
}

static inline void __arch_cpu_intr_on()
{
	intr_on();
}

static inline int __arch_cpu_intr_get()
{
	return intr_get();
}

#endif /* __ACO_ASM_CPU_H */
