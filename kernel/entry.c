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
	ret = buddy_slfcheck();
	ASSERT(ret == 0);

	struct pageframe* test = bkp_alloc(4);
	test = bkp_alloc(7);
	test = bkp_alloc(5);
	test = bkp_alloc(-1);
	aco_module_init();
	ret = buddy_slfcheck();
	ASSERT(ret == 0);
	log_info("hello world!");
}
