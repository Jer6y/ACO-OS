#include "rttest.h"
#include <aco/errno.h>
#include <mm/buddy.h>

#define CHECK(condition)        do                              \
                                {                               \
                                        if(condition)           \
                                                (*success)++;   \
                                        else                    \
                                                (*error)++;     \
                                } while(0)

static void* test_storge[500];
static int test_ptr;

int rt_buddy_api(int* success, int* error)
{
	test_ptr = 0;
	CHECK(MAX_ORDER == CONFIG_BUDDYSYSTEM_MAX_ORDER);
	CHECK(ORDER2PGNUM(0) ==1);
	CHECK(ORDER2PGNUM(1) ==2);
	CHECK(ORDER2PGNUM(2) ==4);
	for(int i=0;i<10;i++)
		CHECK(ORDER2PGNUM(i) ==(1<<i));
	for(int i=0;i<500;i++)
	{
		struct pageframe* tmp = bkp_alloc(i%MAX_ORDER);
		if(tmp != NULL)
		{
			test_storge[test_ptr] = (void*)tmp;
			test_ptr++;
		}
		CHECK(buddy_slfcheck() ==0);
	}
	for(int i=0;i<test_ptr;i++)
	{
		bkp_free((pageframe_t*)(test_storge[i]));
		test_storge[i] = NULL;
		CHECK(buddy_slfcheck() ==0);
	}
	bkp_free(NULL);
	CHECK(1==1);
	test_ptr =0;
	for(int i=0;i<500;i++)
        {
                void* tmp = bk_alloc(i%MAX_ORDER);
                if(tmp != NULL)
                {
                        test_storge[test_ptr] = tmp;
                        test_ptr++;
                }
                CHECK(buddy_slfcheck() ==0);
        }
        for(int i=0;i<test_ptr;i++)
        {
                bk_free(test_storge[i]);
                CHECK(buddy_slfcheck() ==0);
        }
	return 0;
}
