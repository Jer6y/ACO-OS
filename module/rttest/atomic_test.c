#include "rttest.h"
#include <aco/errno.h>
#include <aco/atomic.h>
#include <aco/cpu.h>
#include <aco/log.h>
static atomic_t test_done_cpu =0;

static atomic_t value = 0x2083;


int rt_atomic_api(int* success, int* error)
{
	(*success)++;
	for(int i=0;i<5000;i++)
		atomic_fetch_and_add(&value, 2);
	for(int i=0;i<5000;i++)
		atomic_fetch_and_sub(&value, 1);

	atomic_fetch_and_add(&test_done_cpu, 1);
	if(atomic_fetch_and_add(&test_done_cpu,0) == CPUS)
	{
		if(atomic_fetch_and_add(&value, 0) == (0x2083 + (2* 5000 - 1* 5000)*CPUS))
			(*success)++;
		else
		{
			log_fatal("%x",value);
			(*error)++;
		}
	}
	return 0;
}
