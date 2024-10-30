#include "rttest.h"
#include <aco/errno.h>
#include <mm/buddy.h>
#include <aco/cpu.h>

#define CHECK(condition)        do                              \
                                {                               \
                                        if(condition)           \
                                                (*success)++;   \
                                        else                    \
                                                (*error)++;     \
                                } while(0)

int rt_buddy_api(int* success, int* error)
{
	int cpuid = cpu_getid();
	CHECK(MAX_ORDER == CONFIG_BUDDYSYSTEM_MAX_ORDER);
	CHECK(ORDER2PGNUM(0) ==1);
	CHECK(ORDER2PGNUM(1) ==2);
	CHECK(ORDER2PGNUM(2) ==4);
	for(int i=0;i<10;i++)
		CHECK(ORDER2PGNUM(i) ==(1<<i));
	for(int i=0;i<2000;i++)
	{
		struct pageframe* tmp = bkp_alloc(i%MAX_ORDER);
		CHECK(buddy_slfcheck() ==0);
		bkp_free(tmp);
		CHECK(buddy_slfcheck() ==0);
	}
	bkp_free(NULL);
	CHECK(1==1);
	for(int i=0;i<2000;i++)
        {
                void* tmp = bk_alloc(i%MAX_ORDER);
                CHECK(buddy_slfcheck() ==0);
		bk_free(tmp);
		CHECK(buddy_slfcheck() ==0);
        }
	CHECK(buddy_slfcheck() ==0);
	return 0;
}
