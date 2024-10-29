#ifndef __ACO_MM_KERNEL_MAP_H
#define __ACO_MM_KERNEL_MAP_H

#ifndef __ASSEMBLY__ /* for C */

#include <aco/types.h>
struct kernel_mapping {
	phyaddr_t       pa_st_memory;
	size_t		size_memory;
	phyaddr_t 	pa_st_kernel;
	size_t		size_kernel;
	phyaddr_t	pa_st_free;
	size_t		size_free;
	uintptr_t	va2pa_offset;
};

extern struct kernel_mapping __map_of_kernel;

#define RISCV_VA2PA_OFFSET	((uintptr_t)(__map_of_kernel.va2pa_offset))
#define RISCV_KERNEL_PA_START	((phyaddr_t)(__map_of_kernel.pa_st_kernel))
#define RISCV_KERNEL_PA_END	((phyaddr_t)((uintptr_t)(__map_of_kernel.pa_st_kernel) + (uintptr_t)(__map_of_kernel.size_kernel)))
#define RISCV_KERNEL_VA_START 	((viraddr_t)((uintptr_t)(RISCV_KERNEL_PA_START) + RISCV_VA2PA_OFFSET))
#define RISCV_KERNEL_VA_END	((viraddr_t)((uintptr_t)(RISCV_KERNEL_VA_START) + __map_of_kernel.size_kernel))
#define RISCV_KERNEL_SIZE	((size_t)(__map_of_kernel.size_kernel))

#define RISCV_MEMORY_PA_START	((phyaddr_t)__map_of_kernel.pa_st_memory)
#define RISCV_MEMORY_PA_END	((phyaddr_t)((uintptr_t)__map_of_kernel.pa_st_memory + (uintptr_t)__map_of_kernel.size_memory))
#define RISCV_MEMORY_VA_START	((viraddr_t)((uintptr_t)RISCV_MEMORY_PA_START + RISCV_VA2PA_OFFSET))
#define RISCV_MEMORY_VA_END	((viraddr_t)((uintptr_t)RISCV_MEMORY_VA_START + (uintptr_t)__map_of_kernel.size_memory))
#define RISCV_MEMORY_SIZE	((size_t)__map_of_kernel.size_memory)

#define RISCV_FREE_PA_START	((phyaddr_t)__map_of_kernel.pa_st_free)
#define RISCV_FREE_PA_END	((phyaddr_t)((uintptr_t)__map_of_kernel.pa_st_free + (uintptr_t)__map_of_kernel.size_free))
#define RISCV_FREE_VA_START	((viraddr_t)((uintptr_t)RISCV_FREE_PA_START + RISCV_VA2PA_OFFSET))
#define RISCV_FREE_VA_END	((viraddr_t)((uintptr_t)RISCV_FREE_PA_END + RISCV_VA2PA_OFFSET))
#define RISCV_FREE_SIZE		((size_t)__map_of_kernel.size_free)

void  kernel_map_init(uintptr_t dtb_address);

void* freemem_alloc_pa(void);


#else  /* for asm */


#endif /* END __ASSEMBLY__ */


#endif /* __ACO_MM_KERNEL_MAP_H */
