#include "rttest.h"
#include <aco/errno.h>
#include <aco/spinlock.h>
#include <aco/cpu.h>
#include <aco/atomic.h>

DEFINE_ATOMIC(test_done_cpu,0);
DEFINE_SPINLOCK(test_lk);

static int test = 0;

int rt_spinlock_api(int* success, int* error)
{
	(*success)++;

	lock(&test_lk);
	for(int i=0;i<1000;i++)
		test++;
	for(int i=0;i<1000;i++)
		test++;
	for(int i=0;i<1000;i++)
		test++;
	unlock(&test_lk);

	atomic_fetch_and_add(&test_done_cpu, 1);
	if(atomic_fetch_and_add(&test_done_cpu, 0) == CPUS)
	{
		lock(&test_lk);
		if(test == 3000 * CPUS)
			(*success)++;
		else
			(*error)++;
		unlock(&test_lk);
	}
	return 0;
}
