#include "rttest.h"
#include <aco/errno.h>
int rt_hello(int* success, int* error)
{
	rttest_printf("hello test is here!\n");
	*success = 1;
	*error   = 0;
	return 0;
}
