#include <aco/types.h>
#include <aco/cpu.h>
#include <sbi/hart_management.h>
#include <aco/linkage.h>

#define RISCV_INIT_STKSIZE      CONFIG_RISCV_INIT_STACKSIZE

#define RISCV_PGSIZE		(1<<CONFIG_PAGE_OFFSET)

uint8_t sys_stack[CPUS * RISCV_INIT_STKSIZE] ALIGN(RISCV_PGSIZE);

extern uintptr_t kernel_pagetable;


uintptr_t boot_harts(uintptr_t start_address)
{
	int cpuid = cpu_getid();
	for(int i=0;i<CPUS;i++)
	{
		if(i == cpuid)
			continue;
		sbi_hart_start((unsigned long)i, (unsigned long)start_address, (unsigned long)kernel_pagetable);
	}
	return kernel_pagetable;
}
