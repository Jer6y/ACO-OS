#include "rttest.h"
#include <aco/types.h>
#include <aco/spinlock.h>

PRIVATE_VAR  spinlock_t lk;
PRIVATE_VAR  int lk_val;

#define ADD_TIMES 	300
#define REAL_THREAD	1

int rt_spinlock(int* success, int* error)
{
	lk_val = 0;
	init_lock(&lk);
	lock(&lk);
	for(int i=0;i<ADD_TIMES;i++)
		lk_val++;
	unlock(&lk);

	rttest_printf("expected lk_val : %d\n",ADD_TIMES*REAL_THREAD);
	rttest_printf("actually lk_val : %d\n",lk_val);
	if(lk_val == ADD_TIMES * REAL_THREAD) 
		(*success)++;
	else
		(*error)++;
	return 0;
}
