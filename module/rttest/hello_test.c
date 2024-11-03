#include "rttest.h"
#include <aco/errno.h>
int rt_hello(int* success, int* error)
{
	(*success)++;
	return 0;
}
