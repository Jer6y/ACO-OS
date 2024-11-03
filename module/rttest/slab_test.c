#include "rttest.h"
#include <aco/errno.h>
#include <aco/atomic.h>
#include <mm/slab.h>
#include <aco/cpu.h>
#include <mm/buddy.h>

#define CHECK(condition) do                              \
                                {                               \
                                        if(condition)           \
                                                (*success)++;   \
                                        else                    \
                                                (*error)++;     \
                                } while(0)


DEFINE_ATOMIC(test_done_cpu, 0);

static kmem_cache_t global_cache;
static void*	    t2[CPUS][5000];

int rt_slab_api(int* success, int* error)
{
	(*success)++;
	if(atomic_fetch_and_add(&test_done_cpu, 0) == 0)
		CHECK(kmem_cache_init(&global_cache,126,MAX_ORDER-1,40,0) == 0);
	int   t2_ptr = 0;
	for(int i=0;i<5000;i++)
        {
                void* a = kmem_obj_alloc(&global_cache);
                if( a != NULL)
                        t2[cpu_getid()][t2_ptr++] = a;
                else
                        break;
        }
	
	for(int i=0;i<t2_ptr;i++)
	{
		((char*)(t2[cpu_getid()][i]))[0]   = 0x23;
		((char*)(t2[cpu_getid()][i]))[107] = 0x34;
		((char*)(t2[cpu_getid()][i]))[53]  = 0x12;
		((char*)(t2[cpu_getid()][i]))[13]  = 0xfe;
		((char*)(t2[cpu_getid()][i]))[29]  = 0xfc;
		((char*)(t2[cpu_getid()][i]))[73]  = 0x45;
		((char*)(t2[cpu_getid()][i]))[83]  = 0x31;
		((char*)(t2[cpu_getid()][i]))[125] = 0xff;
		kmem_obj_free(&global_cache, t2[cpu_getid()][i]);
	}
	kmm_cinfo info;
	kmem_cached_info(NULL, NULL);
	CHECK(1 == 1);
	kmem_cached_info(NULL, &info);
	CHECK(1 == 1);
	kmem_cached_info(&global_cache, NULL);
	CHECK(1 == 1);
	kmem_cache_t test_kmem;
	CHECK(kmem_cache_init(NULL,4,0,2,0) != 0);
	CHECK(kmem_cache_init(NULL,0,0,2,0) != 0);
	CHECK(kmem_cache_init(NULL,-1,0,2,0) != 0);
	CHECK(kmem_cache_init(NULL,4,MAX_ORDER,2,0) != 0);
	CHECK(kmem_cache_init(NULL,4,0,0,0) != 0);
	CHECK(kmem_cache_init(NULL,4,0,-1,0) != 0);
	CHECK(kmem_cache_init(NULL,4,0,3,10) != 0);
	CHECK(kmem_cache_init(NULL,4,0,3,3) != 0);
	CHECK(kmem_cache_init(NULL,4,0,3,-1) != 0);
	CHECK(kmem_cache_init(&test_kmem,4,0,3,-1) != 0);
	CHECK(kmem_cache_init(&test_kmem,4,0,3,10) != 0);
	CHECK(kmem_cache_init(&test_kmem,4,0,-1,3) != 0);
	CHECK(kmem_cache_init(&test_kmem,0,0,5,3) != 0);
	CHECK(kmem_cache_init(&test_kmem,-1,0,5,3) != 0);
	CHECK(kmem_cache_init(&test_kmem,4,MAX_ORDER,5,3) != 0);
	CHECK(kmem_cache_init(&test_kmem,4,MAX_ORDER+1,5,3) != 0);
	CHECK(kmem_cache_init(&test_kmem,4,-1,5,3) != 0);
	kmem_cache_deinit(NULL);
	CHECK( 1 == 1);
	CHECK(kmem_obj_alloc(NULL) == NULL);
	kmem_obj_free(NULL, (void*)0x23451);
	CHECK( 1 == 1);
	kmem_obj_free(NULL, NULL);
	CHECK( 1 == 1);
	CHECK(kmem_cache_init(&test_kmem, PAGE_SIZE, 0, 10, 1) != 0);
	CHECK(kmem_cache_init(&test_kmem, PAGE_SIZE-1, 0, 10, 1) != 0);
#if (CONFIG_PAGE_OFFSET == 12)
	void* tmp[28];
	int   ptr = 0;
	CHECK(kmem_cache_init(&test_kmem, 4095, 0, 10, 1) != 0);
	CHECK(kmem_cache_init(&test_kmem, 1542, 0, 10, 1) == 0);
	CHECK(test_kmem.obj_size == 1542);
	CHECK(test_kmem.pg_order == 0);
	CHECK(test_kmem.pg_orlimit == 10);
	CHECK(test_kmem.pg_orhas == 1);
	kmm_cinfo t_info;
        kmem_cached_info(&test_kmem, &info);
	CHECK(test_kmem.obj_size == info.obj_size);
	CHECK(test_kmem.obj_num == info.obj_num);
	CHECK(test_kmem.pg_order == info.pg_order);
	CHECK(test_kmem.pg_orlimit == info.pg_orlimit);
	CHECK(test_kmem.pg_orhas == info.pg_orhas);
	CHECK(list_empty(&(test_kmem.free)) == 0);
	CHECK(list_empty(&(test_kmem.full)) == 1);
	CHECK(list_empty(&(test_kmem.partial)) == 1);
	int objnum = test_kmem.obj_num * 10;
	for(int i=0;i<28;i++)
	{
		void* a = kmem_obj_alloc(&test_kmem);
		if( a != NULL)
			tmp[ptr++] = a;
		else
			break;
	}
	CHECK(ptr != 28);
	kmem_obj_free(&test_kmem, (void*)0);
	CHECK(1 == 1);
	kmem_obj_free(&test_kmem, (void*)1234);
	CHECK(1 == 1);
	
	for(int i=0;i<ptr;i++)
		kmem_obj_free(&test_kmem, tmp[i]);
	CHECK(test_kmem.obj_num == objnum);
	kmem_cache_deinit(&test_kmem);
	CHECK(1 == 1);
#endif
	atomic_fetch_and_add(&test_done_cpu, 1);
        if(atomic_fetch_and_add(&test_done_cpu, 0) == CPUS)
	{
		kmem_cache_deinit(&global_cache);
		CHECK(1==1);
	}
	return 0;
}
