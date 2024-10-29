#include <aco/types.h>
#include <aco/init.h>
#include <aco/log.h>
#include <aco/libfdt.h>
#include <aco/assert.h>
#include <mm/pageframe.h>
#include <mm/buddy.h>

void start_kernel(void)
{
	int ret;
	ret = log_module_init();
	ASSERT(ret ==0);
	ret = fdt_init();
	ASSERT(ret == 0);
	ret = pageframe_init();
	ASSERT(ret == 0);
	ret = buddy_init();
	ASSERT(ret == 0);
	aco_module_init();
	log_info("hello world!");
}
