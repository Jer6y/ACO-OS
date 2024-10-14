#ifndef __RISCV_ASM_FIX_ADDRESS_H
#define __RISCV_ASM_FIX_ADDRESS_H

#include <aco/assembly.h>
#include <aco/linkage.h>
#include <generated/autoconf.h>

#ifndef CONFIG_PHY_MEMORY_BASE
  #error "unkown error! CONFIG_PHY_MEMORY_BASE not configured"
#endif

#ifdef __ASSEMBLY__
	#define FIX_REGISTER(reg,tmp_reg)  	li 	tmp_reg, KERNEL_OFFSET		ASM_NEWLINE\
						sub 	reg, reg, tmp_reg		ASM_NEWLINE\
						li 	tmp_reg, CONFIG_PHY_MEMORY_BASE ASM_NEWLINE\
						add 	reg, reg, tmp_reg
#else
	#define fix_address(addrs) 	((uintptr_t)CONFIG_PHY_MEMORY_BASE + ((uintptr_t)(addrs) - (uintptr_t)KERNEL_OFFSET))
	#define w_global_var(var,new_v)	do 	 					\
					{						\
						uintptr_t var_ptr = (uintptr_t)(&(var));\
						var_ptr = fix_address(var_ptr);		\
						*((typeof((&(var))))var_ptr) = new_v;	\
					} while(0)
						
	#define r_global_var(var)	({						\
						typeof(var) __result;			\
						uintptr_t var_ptr = (uintptr_t)(&(var));\
                                                var_ptr = fix_address(var_ptr);         \
                                                __result = *((typeof((&(var))))var_ptr);\
						__result;				\
					})

	#define w_global_ptr(ptr, val) 	do						\
				       	{						\
						uintptr_t var_ptr = (uintptr_t)(ptr);	\
						var_ptr = fix_address(var_ptr);		\
						*((typeof((ptr)))(var_ptr)) = val;	\
					} while(0)

	#define r_global_ptr(ptr)	({						\
						typeof(*(ptr)) __result;		\
						uintptr_t var_ptr = (uintptr_t)(ptr); 	\
						var_ptr = fix_address(var_ptr);         \
						__result = *((typeof((ptr)))(var_ptr));	\
						__result;				\
					})
#endif /* __ASSEMBLY__ */

#endif /* __RISCV_ASM_FIX_ADDRESS_H */
