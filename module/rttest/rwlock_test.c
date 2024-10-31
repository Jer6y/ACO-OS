#include "rttest.h"
#include <aco/errno.h>
#include <aco/rwlock.h>
#include <aco/atomic.h>
#include <aco/cpu.h>

DEFINE_ATOMIC(test_done_cpu, 0);
DEFINE_RWLOCK(rw_testlk);

static int test =0;

int rt_rwlock_api(int* success, int* error)
{
	(*success)++;
	for(int i=0;i<5000;i++)
	{
		r_lock(&rw_testlk);
		(void)test;
		r_unlock(&rw_testlk);
		w_lock_br(&rw_testlk);
		test = test + 2;
		w_unlock_br(&rw_testlk);
		r_lock(&rw_testlk);
		(void)test;
		r_unlock(&rw_testlk);
		w_lock(&rw_testlk);
		test = test + 2;
		w_unlock(&rw_testlk);
		r_lock(&rw_testlk);
		(void)test;
		r_unlock(&rw_testlk);
	}

	atomic_fetch_and_add(&test_done_cpu, 1);
	if(atomic_fetch_and_add(&test_done_cpu, 0) == CPUS)
	{
		w_lock(&rw_testlk);
		if(test == 4*5000*CPUS)
			(*success)++;
		else
			(*error)++;
		w_unlock(&rw_testlk);
	}
	return 0;
}
