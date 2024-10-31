#include <aco/types.h>
#include <aco/init.h>
#include <aco/log.h>
#include <aco/libfdt.h>
#include <aco/assert.h>
#include <mm/pageframe.h>
#include <mm/buddy.h>
#include <mm/slab.h>
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

#ifdef CONFIG_RTTEST

extern int rt_slab_api(int* success, int* error);
extern int rt_buddy_api(int* success, int* error);
extern int rt_atomic_api(int* success, int* error);
extern int rt_rwlock_api(int* success, int* error);
extern int rt_spinlock_api(int* success, int* error);

typedef int (*test_handler)(int*,int*);

static struct {
    const char* 	name;
    test_handler	test_callbk;
} smp_entrys[] = 
{
#if (CONFIG_TEST_BUDDY_API == 1)
	{ "smp buddy test", rt_buddy_api},
#endif

#if (CONFIG_TEST_RWLOCK_API == 1)
	{ "smp rwlok test", rt_rwlock_api},
#endif

#if (CONFIG_TEST_SPINLOCK_API == 1)
	{ "smp splok test", rt_spinlock_api},
#endif

#if (CONFIG_TEST_SLAB_API == 1)
	{ "smp slab  test", rt_slab_api},
#endif

#if (CONFIG_TEST_ATOMIC_API == 1)
	{ "smp atmic test", rt_atomic_api},
#endif

};

static int smp_test()
{
	int cpuid = cpu_getid();
	for(int i=0;i<sizeof(smp_entrys) / sizeof(smp_entrys[0]);i++)
	{
		int suc   = 0;
		int err   = 0;
		smp_entrys[i].test_callbk(&suc,&err);
		log_debug("[CPU %d]: %s %d/%d",cpuid, smp_entrys[i].name, suc, suc+err);
	}
	return 0;
}

#endif

static void* x[500];
static int ptr =0;

void start_kernel(void* args)
{
	if(cpu_getid() == cpu_boot)
	{
		int ret;
		ret = log_module_init();
		ASSERT(ret == 0);
#ifdef CONFIG_LIB_FDT
		uintptr_t dtb = (uintptr_t)args;
		ret = fdt_init(dtb);
		ASSERT(ret == 0);
#endif
		ret = pageframe_init();
		ASSERT(ret == 0);
		ret = buddy_init();
		ASSERT(ret == 0);
		ret = kmem_init();
		ASSERT(ret == 0);
		aco_module_init();
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
		while(atomic_fetch_and_add(&module_inited,0) ==0) ;
#ifdef CONFIG_RTTEST
		ret = smp_test();
		ASSERT(ret == 0);
#endif
		atomic_fetch_and_add(&bootdone_cpus,1);
	}
}
