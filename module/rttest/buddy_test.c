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

int rt_buddy_api(int* success, int* error)
{
	CHECK(MAX_ORDER == CONFIG_BUDDYSYSTEM_MAX_ORDER);
	CHECK(ORDER2PGNUM(0) ==1);
	CHECK(ORDER2PGNUM(1) ==2);
	CHECK(ORDER2PGNUM(2) ==4);
	CHECK(ORDER2BYTE(0) == 1*PAGE_SIZE);
	CHECK(ORDER2BYTE(1) == 2*PAGE_SIZE);
	CHECK(ORDER2BYTE(2) == 4*PAGE_SIZE);
	CHECK(ORDER2BYTE(3) == 8*PAGE_SIZE);
	CHECK(PGNUM2ORDER(0) == -1);
	CHECK(PGNUM2ORDER(-3) == -1);
	CHECK(PGNUM2ORDER(-4) == -1);
	CHECK(PGNUM2ORDER(1) == 0);
#if (CONFIG_BUDDYSYSTEM_MAX_ORDER >= 2)
	CHECK(PGNUM2ORDER(2) == 1);
	CHECK(PGNUM2ORDER(3) == -1);
#endif

#if (CONFIG_BUDDYSYSTEM_MAX_ORDER >= 3)
	CHECK(PGNUM2ORDER(4) == 2);
	CHECK(PGNUM2ORDER(5) == -1);
	CHECK(PGNUM2ORDER(7) == -1);
#endif

#if (CONFIG_BUDDYSYSTEM_MAX_ORDER >= 4)
	CHECK(PGNUM2ORDER(8) == 3);
#endif

#if (CONFIG_BUDDYSYSTEM_MAX_ORDER >= 5)
	CHECK(PGNUM2ORDER(16) == 4);
#endif

#if (CONFIG_BUDDYSYSTEM_MAX_ORDER >= 6)
	CHECK(PGNUM2ORDER(32) == 5);
#endif

#if (CONFIG_BUDDYSYSTEM_MAX_ORDER >= 7)
	CHECK(PGNUM2ORDER(64) == 6);
	CHECK(PGNUM2ORDER(63) == -1);
#endif
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
