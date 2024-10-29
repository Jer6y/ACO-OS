#include "rttest.h"
#include <aco/errno.h>
#include <mm/memblock_alloc.h>

#define CHECK(condition)        do                              \
                                {                               \
                                        if(condition)           \
                                                (*success)++;   \
                                        else                    \
                                                (*error)++;     \
                                } while(0)

int rt_memblock_api(int* success, int* error)
{
#ifdef CONFIG_MEM_BLOCK_ALOCTOR
	CHECK(memblock_allocator_init(NULL,0,0,0) !=0);
        CHECK(memblock_alloc(NULL) == NULL);
        memblock_free(NULL,NULL);
        CHECK(memblock_for_each(NULL,NULL) !=0);
        CHECK(memblock_for_each_safe(NULL,NULL) !=0);
        CHECK(memblock_allocator_merge(NULL,NULL) == 0);
        CHECK(memblock_self_check(NULL) != 0);
        CHECK(memblock_self_check_with_output(NULL,NULL) !=0);
#else
	CHECK(memblock_allocator_init(NULL,0,0,0) ==0);
        CHECK(memblock_alloc(NULL) == NULL);
        memblock_free(NULL,NULL);
        CHECK(memblock_for_each(NULL,NULL) ==0);
        CHECK(memblock_for_each_safe(NULL,NULL) ==0);
        CHECK(memblock_allocator_merge(NULL,NULL) == 0);
        CHECK(memblock_self_check(NULL) == 0);
        CHECK(memblock_self_check_with_output(NULL,NULL) ==0);
#endif

	return 0;

}
