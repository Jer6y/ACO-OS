#ifndef __ASM_SPINLOCK_H
#define __ASM_SPINLOCK_H

#include <aco/types.h>
#include <csr.h>
struct __arch_lock_s {
	uint8 int_enable;
};

#define __arch_init_local_lock(ptr)  	((ptr)->int_enable = 0)

#define __arch_local_lock(ptr)		do  {						\
						(ptr)->int_enable = csr_intlock();	\
					}  while(0)

#define __arch_local_unlock(ptr)	do  {						\
						csr_intrestore((ptr)->int_enable);	\
					}  while(0)

#endif /* __ASM_SPINLOCK_H */
