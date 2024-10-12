#include <sbi/debug_console.h>
#include <aco/init.h>
#include <printf.h>
char early_stack[8192];
void main(void)
{
	aco_module_init();
}

int test_init(void)
{
	riscv_printf("Hello, %s,%d,%c,%f\n","123",234,'a',2.342);
	return 0;
}

REGISTER_MODULE_INIT(4, test_init);

