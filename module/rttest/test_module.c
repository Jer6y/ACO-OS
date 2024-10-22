#include <aco/types.h>
#include <aco/init.h>
#include <aco/link.h>
#include <generated/autoconf.h>
#include "rttest.h"

#define REGISTER_TEST_FUNC(describe, function)	{ describe, function }
#define REGISTER_EXTERN_FUNC(function)	extern int function(int*,int*)


REGISTER_EXTERN_FUNC(rt_hello);
REGISTER_EXTERN_FUNC(rt_libstr);
REGISTER_EXTERN_FUNC(rt_init_section);
REGISTER_EXTERN_FUNC(rt_list_api);
REGISTER_EXTERN_FUNC(rt_page_api);

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
};


FUNC_BUILTIN int test_module_init(void)
{
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
	return 0;
}


FUNC_BUILTIN void test_module_exit(void)
{
	return;
}


REGISTER_MODULE_INIT(PRIO_LOWEST, test_module_init);
REGISTER_MODULE_EXIT(PRIO_LOWEST, test_module_exit);
