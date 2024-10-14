#include <aco/types.h>
#include <aco/init.h>
#include <aco/linkage.h>

#include "rttest.h"

#define REGISTER_TEST_FUNC(describe, function)	{ describe, function }
#define REGISTER_EXTERN_FUNC(function)	extern int function(int*,int*)


REGISTER_EXTERN_FUNC(rt_hello);
REGISTER_EXTERN_FUNC(rt_spinlock);

PRIVATE_VAR test_entry_t t_entrys[] =
{                                  
	REGISTER_TEST_FUNC("hello test", rt_hello),
	REGISTER_TEST_FUNC("splok test", rt_spinlock),
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
		rttest_printf("=================================================\n");
	}
	return 0;
}


FUNC_BUILTIN void test_module_exit(void)
{
	return;
}


REGISTER_MODULE_INIT(PRIO_MIN, test_module_init);
REGISTER_MODULE_EXIT(PRIO_MIN, test_module_exit);
