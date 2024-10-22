#include <aco/init.h>
#include <aco/log.h>
#include <mm/early_mm.h>

void main(void)
{
	aco_module_init();
	log_info("hello world!");
	//mem_test();
}
