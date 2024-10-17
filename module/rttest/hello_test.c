#include "rttest.h"
#include <aco/errno.h>
int rt_hello(int* success, int* error)
{
	*success = 1;
	*error   = 0;
	return 0;
}
