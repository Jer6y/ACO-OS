#include <aco/types.h>
#include <aco/init.h>
#include <aco/log.h>
#include <aco/libfdt.h>
#include <aco/assert.h>
#include <mm/pageframe.h>
#include <mm/buddy.h>
#include <mm/slab.h>
#include <aco/cpu.h>
#include <mm/vm.h>

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

void start_kernel(void* args)
{
	if(cpu_getid() == cpu_boot)
	{
		int ret = aco_module_init();
		ASSERT(ret == 0);
		atomic_fetch_and_add(&module_inited,1);
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
		while(atomic_fetch_and_add(&module_inited,0) ==0);
		ret = aco_module_init();
		ASSERT(ret == 0);
		atomic_fetch_and_add(&bootdone_cpus,1);
	}
}
