#include <aco/types.h>
#include <aco/init.h>
#include <aco/log.h>
#include <aco/libfdt.h>
#include <aco/assert.h>
#include <mm/pageframe.h>
#include <mm/buddy.h>
#include <aco/cpu.h>

uint64_t cpu_boot;
static atomic_t module_inited = 0;
static atomic_t bootdone_cpus = 0;

const  char* aco_logo[] = 
{
	"\e[96m\e[1m     _       ____    ___             ___    ____  ",
	"    / \\     / ___|  / _ \\           / _ \\  / ___| ",
	"   / _ \\   | |     | | | |  _____  | | | | \\___ \\ ",
	"  / ___ \\  | |___  | |_| | |_____| | |_| |  ___) |",
	" /_/   \\_\\  \\____|  \\___/           \\___/  |____/ ",
	"                                                  \e[39m\e[0m",
	NULL,
};


static int smp_test()
{
// we pick up some testcase for rt-test module
// to test if smp work
extern int rt_buddy_api(int* success, int* error);
	int cpuid = cpu_getid();
	int suc   = 0;
	int err   = 0;
	ASSERT(rt_buddy_api(&suc, &err) == 0);
	log_debug("[CPU %d]: smp buddy test %d/%d",cpuid, suc, suc+err);
	return 0;
}


void start_kernel(void)
{
	if(cpu_getid() == cpu_boot)
	{
		int ret;
		ret = log_module_init();
		ASSERT(ret == 0);
		ret = fdt_init();
		ASSERT(ret == 0);
		ret = pageframe_init();
		ASSERT(ret == 0);
		ret = buddy_init();
		ASSERT(ret == 0);
		aco_module_init();
		atomic_fetch_and_add(&module_inited,1);
		ret = smp_test();
		ASSERT(ret == 0);
		atomic_fetch_and_add(&bootdone_cpus,1);
		while(atomic_fetch_and_add(&bootdone_cpus,0) != CPUS) ;
		ret = pages_slfcheck();
		ASSERT(ret == 0);
		for(int i=0;aco_logo[i] != NULL;i++)
			puts(aco_logo[i]);
	}
	else
	{
		int ret;
		while(atomic_fetch_and_add(&module_inited,0) ==0) ;
		ret = smp_test();
		ASSERT(ret == 0);
		atomic_fetch_and_add(&bootdone_cpus,1);
	}
}
