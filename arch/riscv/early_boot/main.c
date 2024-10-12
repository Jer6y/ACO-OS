#include <sbi/debug_console.h>
#include <aco/init.h>
#include <printf.h>
char early_stack[8192];
//static volatile int start = 0;
//static volatile int cold_boot_end = 0;
void main(void)
{
	aco_module_init();
	//asm volatile("":::"memory");
	//start = 1;
	//while(cold_boot_end != 1) ;
}

/*
void cold_boot(void)
{
	riscv_printf("cold boot!\n");
	while(start != 1) ;
	check_spin();
	cold_boot_end++;
	return;
}
*/


