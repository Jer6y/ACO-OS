#include <aco/types.h>
#include <aco/init.h>
#include <aco/link.h>
#include <generated/autoconf.h>
#include <aco/atomic.h>
#include <aco/cpu.h>
#include <aco/log.h>
#include "rttest.h"


DEFINE_ATOMIC(test_init_done,0);

#define REGISTER_TEST_FUNC(describe, function)	{ describe, function }
#define REGISTER_EXTERN_FUNC(function)	extern int function(int*,int*)


REGISTER_EXTERN_FUNC(rt_hello);
REGISTER_EXTERN_FUNC(rt_libstr);
REGISTER_EXTERN_FUNC(rt_init_section);
REGISTER_EXTERN_FUNC(rt_list_api);
REGISTER_EXTERN_FUNC(rt_page_api);
REGISTER_EXTERN_FUNC(rt_pageframe_api);
REGISTER_EXTERN_FUNC(rt_cpu_api);
REGISTER_EXTERN_FUNC(rt_memblock_api);
REGISTER_EXTERN_FUNC(rt_buddy_api);
REGISTER_EXTERN_FUNC(rt_atomic_api);
REGISTER_EXTERN_FUNC(rt_spinlock_api);
REGISTER_EXTERN_FUNC(rt_rwlock_api);
REGISTER_EXTERN_FUNC(rt_slab_api);
REGISTER_EXTERN_FUNC(rt_avlmini);
REGISTER_EXTERN_FUNC(rt_vma_api);

PRIVATE_VAR test_entry_t t_entrys[] =
{                                  
#if (CONFIG_TEST_HELLO == 1)
	REGISTER_TEST_FUNC("hello_world test", rt_hello),
#endif

#if (CONFIG_TEST_LIBSTR == 1)
	REGISTER_TEST_FUNC("lib_str     test", rt_libstr),
#endif

#if (CONFIG_TEST_INIT_SECTION == 1)
	REGISTER_TEST_FUNC("init_secton test", rt_init_section),
#endif

#if (CONFIG_TEST_LIST_API == 1)
	REGISTER_TEST_FUNC("list_api    test", rt_list_api),
#endif

#if (CONFIG_TEST_PAGE_API == 1)
	REGISTER_TEST_FUNC("page_api    test", rt_page_api),
#endif

#if (CONFIG_TEST_PGFM_API == 1)
	REGISTER_TEST_FUNC("pgframe_api test", rt_pageframe_api),
#endif

#if (CONFIG_TEST_CPU_API == 1)
	REGISTER_TEST_FUNC("cpu_lib_api test", rt_cpu_api),
#endif

#if (CONFIG_TEST_MEMBLOCK_API == 1)
	REGISTER_TEST_FUNC("memblok api test", rt_memblock_api),
#endif

#if (CONFIG_TEST_BUDDY_API == 1)
	REGISTER_TEST_FUNC("buddy alloc test", rt_buddy_api),
#endif

#if (CONFIG_TEST_ATOMIC_API == 1)
	REGISTER_TEST_FUNC("atomic  api test", rt_atomic_api),
#endif

#if (CONFIG_TEST_SPINLOCK_API == 1)
	REGISTER_TEST_FUNC("spinlok api test", rt_spinlock_api),
#endif

#if (CONFIG_TEST_RWLOCK_API == 1)
	REGISTER_TEST_FUNC("rwlock  api test", rt_rwlock_api),
#endif

#if (CONFIG_TEST_SLAB_API == 1)
	REGISTER_TEST_FUNC("slab    api test", rt_slab_api),
#endif

#if (CONFIG_TEST_AVL_API == 1)
	REGISTER_TEST_FUNC("avltree api test", rt_avlmini),
#endif

#if (CONFIG_TEST_VMA_API == 1)
	REGISTER_TEST_FUNC("vma_are api test", rt_vma_api),
#endif
};

PRIVATE_VAR test_entry_t smp_entrys[] =
{
#if (CONFIG_TEST_BUDDY_API == 1)
	REGISTER_TEST_FUNC("smp buddy test", rt_buddy_api),
#endif

#if (CONFIG_TEST_RWLOCK_API == 1)
	REGISTER_TEST_FUNC("smp rwlok test", rt_rwlock_api),
#endif

#if (CONFIG_TEST_SPINLOCK_API == 1)
	REGISTER_TEST_FUNC("smp splok test", rt_spinlock_api),
#endif

#if (CONFIG_TEST_SLAB_API == 1)
	REGISTER_TEST_FUNC("smp slab  test", rt_slab_api),
#endif

#if (CONFIG_TEST_ATOMIC_API == 1)
	REGISTER_TEST_FUNC("smp atmic test", rt_atomic_api),
#endif
};


FUNC_BUILTIN int smp_test()
{
        int cpuid = cpu_getid();
        for(int i=0;i<sizeof(smp_entrys) / sizeof(smp_entrys[0]);i++)
        {
                int suc   = 0;
                int err   = 0;
                smp_entrys[i].rttest_func(&suc,&err);
                log_debug("[CPU %d]: %s %d/%d",cpuid, smp_entrys[i].describe, suc, suc+err);
        }
        return 0;
}

FUNC_BUILTIN int test_module_init(void)
{
	if(atomic_fetch_and_add(&test_init_done,0) != 0)
	{
		return smp_test();
	}
	int total_success = 0;
	int total = 0;
	int total_error = 0;
	int rttest_size = sizeof(t_entrys) / sizeof(t_entrys[0]);
	for(int i=0; i<rttest_size; i++)
	{
		int success = 0;
		int error = 0;
		rttest_printf("===============test for %s===============\n", t_entrys[i].describe);
		t_entrys[i].rttest_func(&success, &error);
		total += success;
		total += error;
		total_success += success;
		total_error += error;
		rttest_printf("test result for %s: %d/%d \n", t_entrys[i].describe, success, success + error);
		rttest_printf("=======================================================\n");
	}
	atomic_fetch_and_add(&test_init_done, 1);
	return 0;
}


FUNC_BUILTIN void test_module_exit(void)
{
	return;
}


REGISTER_MODULE_INIT(PRIO_31, test_module_init);
REGISTER_MODULE_EXIT(PRIO_31, test_module_exit);
