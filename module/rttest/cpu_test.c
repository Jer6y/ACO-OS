#include "rttest.h"
#include <aco/errno.h>
#include <aco/cpu.h>
#include <aco/cpu_critical.h>


#define CHECK(condition)        do                              \
                                {                               \
                                        if(condition)           \
                                                (*success)++;   \
                                        else                    \
                                                (*error)++;     \
                                } while(0)


int rt_cpu_api(int* success, int* error)
{
	//CHECK(cpu_getid() == 0);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_out() == 0);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_in() == 1);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_in() == 2);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_in() == 3);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_in() == 4);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_in() == 5);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_in() == 6);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_in() == 7);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_in() == 8);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_in() == 9);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_in() == 10);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_out() == 9);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_out() == 8);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_out() == 7);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_out() == 6);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_out() == 5);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_out() == 4);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_out() == 3);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_out() == 2);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_out() == 1);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_out() == 0);
	CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_out() == 0);
	CHECK(cpu_intr_get() == 0);
	/*
	CHECK(cpu_intr_get() == 0);
	cpu_intr_on();
	CHECK(cpu_intr_get() == 1);
	cpu_intr_off();
	CHECK(cpu_intr_get() == 0);
	cpu_intr_restore(1);
	CHECK(cpu_intr_get() == 1);
	cpu_intr_restore(0);
	CHECK(cpu_intr_get() == 0);
	cpu_intr_restore(1);
	CHECK(cpu_intr_get() == 1);
	CHECK(cpu_critical_out() == 0);
        CHECK(cpu_intr_get() == 1);
	CHECK(cpu_critical_out() == 0);
        CHECK(cpu_intr_get() == 1);
	CHECK(cpu_critical_out() == 0);
        CHECK(cpu_intr_get() == 1);
	CHECK(cpu_critical_out() == 0);
        CHECK(cpu_intr_get() == 1);
	CHECK(cpu_critical_out() == 0);
        CHECK(cpu_intr_get() == 1);
	CHECK(cpu_critical_in() == 1);
        CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_in() == 2);
        CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_in() == 3);
        CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_out() == 2);
        CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_out() == 1);
        CHECK(cpu_intr_get() == 0);
	CHECK(cpu_critical_out() == 0);
        CHECK(cpu_intr_get() == 1);
	CHECK(cpu_critical_out() == 0);
        CHECK(cpu_intr_get() == 1);
	*/
	return 0;
}
