#include "rttest.h"
#include <aco/errno.h>
#include <mm/pageframe.h>
#include <aco/linkage.h>

#define CHECK(condition)        do                              \
                                {                               \
                                        if(condition)           \
                                                (*success)++;   \
                                        else                    \
                                                (*error)++;     \
                                } while(0)

int rt_pageframe_api(int* success, int* error)
{
	CHECK(PGFRAME_VA_KSTART == KERNEL_OFFSET);
	CHECK(VAL_IS_PAGE_ALIGNED(PGFRAME_PA_START) == 1);
	CHECK(VAL_IS_PAGE_ALIGNED(PGFRAME_VA_START) == 1);
	CHECK(VAL_IS_PAGE_ALIGNED(PGFRAME_SIZE) == 1);
	CHECK(VAL_IS_PAGE_ALIGNED(PGFRAME_PA_END) == 1);
	CHECK(VAL_IS_PAGE_ALIGNED(PGFRAME_VA_END) == 1);
	CHECK(VAL_IS_PAGE_ALIGNED(PGFRAME_VA2PA_OFFSET) == 1);
	CHECK(VAL_IS_PAGE_ALIGNED(PGFRAME_PGSIZE) == 1);
	CHECK(PGFRAME_VA_START >= PGFRAME_PA_START);
	CHECK(PGFRAME_VA_KSTART >= PGFRAME_PA_KSTART);
	CHECK((PGFRAME_PA_KSTART - KERNEL_OFFSET) == -PGFRAME_VA2PA_OFFSET);
	CHECK(PGFRAME_PA_START + PGFRAME_SIZE == PGFRAME_PA_END);
	CHECK(PGFRAME_VA_START + PGFRAME_SIZE == PGFRAME_VA_END);
	CHECK(PGFRAME_PA_START + PGFRAME_VA2PA_OFFSET == PGFRAME_VA_START);
	CHECK(PGFRAME_VA_START - PGFRAME_VA2PA_OFFSET == PGFRAME_PA_START);
	CHECK(PGFRAME_PA_END + PGFRAME_VA2PA_OFFSET == PGFRAME_VA_END);
	CHECK(PGFRAME_VA_END - PGFRAME_VA2PA_OFFSET == PGFRAME_PA_END);
	CHECK(PGFRAME_PA_KSTART + PGFRAME_VA2PA_OFFSET == PGFRAME_VA_KSTART);
	CHECK(PGFRAME_VA_KSTART - PGFRAME_VA2PA_OFFSET == PGFRAME_PA_KSTART);
	CHECK(PGFRAME_FREE_SIZE > 0);
	CHECK(PGFRAME_VA2PA(PGFRAME_VA_START) == PGFRAME_PA_START);
	CHECK(PGFRAME_VA2PA(PGFRAME_VA_KSTART) == PGFRAME_PA_KSTART);
	CHECK(PGFRAME_VA2PA(PGFRAME_VA_START) + 1 == PGFRAME_PA_START + 1);
	CHECK(PGFRAME_VA2PA(PGFRAME_VA_KSTART) + 10 == PGFRAME_PA_KSTART + 10);
	CHECK(PGFRAME_VA2PA(PGFRAME_VA_START) + 23 == PGFRAME_PA_START + 23);
	CHECK(PGFRAME_VA2PA(PGFRAME_VA_KSTART) -323 == PGFRAME_PA_KSTART -323);
	CHECK(PGFRAME_VA2PA(PGFRAME_VA_START) -4871723 == PGFRAME_PA_START -4871723);
	CHECK(PGFRAME_VA2PA(PGFRAME_VA_KSTART) -37 == PGFRAME_PA_KSTART -37);
	
	CHECK(PGFRAME_PA2VA(PGFRAME_PA_START) == PGFRAME_VA_START);
	CHECK(PGFRAME_PA2VA(PGFRAME_PA_KSTART) == PGFRAME_VA_KSTART);
	CHECK(PGFRAME_PA2VA(PGFRAME_PA_START) + 1 == PGFRAME_VA_START + 1);
	CHECK(PGFRAME_PA2VA(PGFRAME_PA_KSTART) + 10 == PGFRAME_VA_KSTART + 10);
	CHECK(PGFRAME_PA2VA(PGFRAME_PA_START) + 23 == PGFRAME_VA_START + 23);
	CHECK(PGFRAME_PA2VA(PGFRAME_PA_KSTART) -323 == PGFRAME_VA_KSTART -323);
	CHECK(PGFRAME_PA2VA(PGFRAME_PA_START) -4871723 == PGFRAME_VA_START -4871723);
	CHECK(PGFRAME_PA2VA(PGFRAME_PA_KSTART) -37 == PGFRAME_VA_KSTART -37);

	CHECK(PGFRAME_PA2START_OFFSET(PGFRAME_PA_START) == 0);
	CHECK(PGFRAME_PA2START_OFFSET(PGFRAME_PA_START) + 10 == 10);
	CHECK(PGFRAME_PA2START_OFFSET(PGFRAME_PA_START) + 3812 == 3812);
	CHECK(PGFRAME_PA2START_OFFSET(PGFRAME_PA_START) - 3318812 == -3318812);
	CHECK(PGFRAME_PA2START_OFFSET(PGFRAME_VA_START) == PGFRAME_VA2PA_OFFSET);
	CHECK(PGFRAME_VA2START_OFFSET(PGFRAME_VA_START) == 0);
	CHECK(PGFRAME_VA2START_OFFSET(PGFRAME_VA_START) + 10 == 10);
	CHECK(PGFRAME_VA2START_OFFSET(PGFRAME_VA_START) + 3812 == 3812);
	CHECK(PGFRAME_VA2START_OFFSET(PGFRAME_VA_START) - 3318812 == -3318812);
	
	CHECK(PGFRAME_PA2PAGEID(PGFRAME_PA_START) == 0);
	CHECK(pa2id(PGFRAME_PA_START) ==0);
#if (CONFIG_PAGE_OFFSET == 12)
	CHECK(PGFRAME_PA2PAGEID(PGFRAME_PA_START +1) ==0);
	CHECK(pa2id(PGFRAME_PA_START + 1) ==0);
	CHECK(PGFRAME_PA2PAGEID(PGFRAME_PA_START +2) ==0);
	CHECK(pa2id(PGFRAME_PA_START + 2) ==0);
	CHECK(PGFRAME_PA2PAGEID(PGFRAME_PA_START +4023) ==0);
	CHECK(pa2id(PGFRAME_PA_START + 4023) ==0);
	CHECK(PGFRAME_PA2PAGEID(PGFRAME_PA_START +4095) ==0);
	CHECK(pa2id(PGFRAME_PA_START + 4095) ==0);
	CHECK(PGFRAME_PA2PAGEID(PGFRAME_PA_START +4096) ==1);
	CHECK(pa2id(PGFRAME_PA_START + 4096) ==1);
	CHECK(PGFRAME_PA2PAGEID(PGFRAME_PA_START +4396) ==1);
	CHECK(pa2id(PGFRAME_PA_START + 4396) ==1);
	CHECK(PGFRAME_PA2PAGEID(PGFRAME_PA_START +8191) ==1);
	CHECK(pa2id(PGFRAME_PA_START + 8191) ==1);
	CHECK(PGFRAME_PA2PAGEID(PGFRAME_PA_START +8192) ==2);
	CHECK(pa2id(PGFRAME_PA_START + 8192) ==2);
#endif 
	CHECK(PGFRAME_VA2PAGEID(PGFRAME_VA_START) == 0);
	CHECK(va2id(PGFRAME_VA_START) ==0);
#if (CONFIG_PAGE_OFFSET == 12)
	CHECK(PGFRAME_VA2PAGEID(PGFRAME_VA_START +1) ==0);
	CHECK(va2id(PGFRAME_VA_START +1) ==0);
	CHECK(PGFRAME_VA2PAGEID(PGFRAME_VA_START +2) ==0);
	CHECK(va2id(PGFRAME_VA_START +2) ==0);
	CHECK(PGFRAME_VA2PAGEID(PGFRAME_VA_START +4023) ==0);
	CHECK(va2id(PGFRAME_VA_START +4023) ==0);
	CHECK(PGFRAME_VA2PAGEID(PGFRAME_VA_START +4095) ==0);
	CHECK(va2id(PGFRAME_VA_START +4095) ==0);
	CHECK(PGFRAME_VA2PAGEID(PGFRAME_VA_START +4096) ==1);
	CHECK(va2id(PGFRAME_VA_START +4096) ==1);
	CHECK(PGFRAME_VA2PAGEID(PGFRAME_VA_START +4396) ==1);
	CHECK(va2id(PGFRAME_VA_START +4396) ==1);
	CHECK(PGFRAME_VA2PAGEID(PGFRAME_VA_START +8191) ==1);
	CHECK(va2id(PGFRAME_VA_START +8191) ==1);
	CHECK(PGFRAME_VA2PAGEID(PGFRAME_VA_START +8192) ==2);
	CHECK(va2id(PGFRAME_VA_START +8192) ==2);
#endif 
	CHECK(PGFRAME_PAGEID2VA(0) == PGFRAME_VA_START);
	CHECK(id2va(0) ==PGFRAME_VA_START);
	CHECK(VAL_IS_PAGE_ALIGNED(PGFRAME_PAGEID2VA(0)) == 1);
	CHECK(PGFRAME_PAGEID2VA(1) == PGFRAME_VA_START + PGFRAME_PGSIZE);
	CHECK(id2va(1) ==PGFRAME_VA_START + PGFRAME_PGSIZE);
	CHECK(VAL_IS_PAGE_ALIGNED(PGFRAME_PAGEID2VA(1)) == 1);
	CHECK(PGFRAME_PAGEID2VA(2) == PGFRAME_VA_START + 2*PGFRAME_PGSIZE);
	CHECK(id2va(2) ==PGFRAME_VA_START + 2*PGFRAME_PGSIZE);
	CHECK(VAL_IS_PAGE_ALIGNED(PGFRAME_PAGEID2VA(2)) == 1);
	CHECK(PGFRAME_PAGEID2VA(381) == PGFRAME_VA_START + 381*PGFRAME_PGSIZE);
	CHECK(id2va(381) ==PGFRAME_VA_START + 381*PGFRAME_PGSIZE);
	CHECK(VAL_IS_PAGE_ALIGNED(PGFRAME_PAGEID2VA(381)) == 1);
#if (CONFIG_PAGE_OFFSET == 12)
	CHECK(PGFRAME_PAGEID2VA(1) == PGFRAME_VA_START + 4096);
	CHECK(id2va(1) ==PGFRAME_VA_START + 4096);
	CHECK(PGFRAME_PAGEID2VA(2) == PGFRAME_VA_START + 8192);
	CHECK(id2va(2) ==PGFRAME_VA_START + 8192);
#endif
	CHECK(PGFRAME_PAGEID2PA(0) == PGFRAME_PA_START);
	CHECK(id2pa(0) ==PGFRAME_PA_START);
	CHECK(VAL_IS_PAGE_ALIGNED(PGFRAME_PAGEID2PA(0)) == 1);
	CHECK(PGFRAME_PAGEID2PA(1) == PGFRAME_PA_START + PGFRAME_PGSIZE);
	CHECK(id2pa(1) ==PGFRAME_PA_START + 1*PGFRAME_PGSIZE);
	CHECK(VAL_IS_PAGE_ALIGNED(PGFRAME_PAGEID2PA(1)) == 1);
	CHECK(PGFRAME_PAGEID2PA(2) == PGFRAME_PA_START + 2*PGFRAME_PGSIZE);
	CHECK(id2pa(2) ==PGFRAME_PA_START + 2*PGFRAME_PGSIZE);
	CHECK(VAL_IS_PAGE_ALIGNED(PGFRAME_PAGEID2PA(2)) == 1);
	CHECK(PGFRAME_PAGEID2PA(381) == PGFRAME_PA_START + 381*PGFRAME_PGSIZE);
	CHECK(id2pa(381) ==PGFRAME_PA_START + 381*PGFRAME_PGSIZE);
	CHECK(VAL_IS_PAGE_ALIGNED(PGFRAME_PAGEID2PA(381)) == 1);
#if (CONFIG_PAGE_OFFSET == 12)
	CHECK(PGFRAME_PAGEID2PA(1) == PGFRAME_PA_START + 4096);
	CHECK(id2pa(1) ==PGFRAME_PA_START + 1*4096);
	CHECK(PGFRAME_PAGEID2PA(2) == PGFRAME_PA_START + 8192);
	CHECK(id2pa(2) ==PGFRAME_PA_START + 2*4096);
#endif
	CHECK(id2pg(0) == PAGES);
	CHECK(id2pg(1) == PAGES + 1);
	CHECK(id2pg(2) == PAGES + 2);
	CHECK(id2pg(1237) == PAGES + 1237);
	CHECK(id2pg(137) == PAGES + 137);
	CHECK(pg2id(PAGES) == 0);
	CHECK(pg2id(PAGES + 1) == 1);
	CHECK(pg2id(PAGES + 2) == 2);
	CHECK(pg2id(PAGES + 3) == 3);
	CHECK(pg2id(PAGES + 137) == 137);
	CHECK(pa2pg(PGFRAME_PA_START) == PAGES);
	CHECK(va2pg(PGFRAME_VA_START) == PAGES);
	CHECK(pa2pg(PGFRAME_PA_START + PGFRAME_PGSIZE -1) == PAGES);
	CHECK(va2pg(PGFRAME_VA_START + PGFRAME_PGSIZE -1) == PAGES);
	CHECK(pa2pg(PGFRAME_PA_START + PGFRAME_PGSIZE) == PAGES +1);
	CHECK(va2pg(PGFRAME_VA_START + PGFRAME_PGSIZE) == PAGES +1);
	CHECK(pa2pg(PGFRAME_PA_START + 2*PGFRAME_PGSIZE -1) == PAGES +1);
	CHECK(va2pg(PGFRAME_VA_START + 2*PGFRAME_PGSIZE -1) == PAGES +1);
	CHECK(pa2pg(PGFRAME_PA_START + 2*PGFRAME_PGSIZE) == PAGES + 2);
	CHECK(va2pg(PGFRAME_VA_START + 2*PGFRAME_PGSIZE) == PAGES + 2);
#if (CONFIG_PAGE_OFFSET == 12)
	CHECK(pa2pg(PGFRAME_PA_START + 1) == PAGES);
        CHECK(va2pg(PGFRAME_VA_START + 1) == PAGES);
	CHECK(pa2pg(PGFRAME_PA_START + 1321) == PAGES);
        CHECK(va2pg(PGFRAME_VA_START + 1321) == PAGES);
	CHECK(pa2pg(PGFRAME_PA_START + 4095) == PAGES);
        CHECK(va2pg(PGFRAME_VA_START + 4095) == PAGES);
	CHECK(pa2pg(PGFRAME_PA_START + 4096) == PAGES +1);
        CHECK(va2pg(PGFRAME_VA_START + 4096) == PAGES +1);
#endif
	CHECK(pg2pa(PAGES + 0) == PGFRAME_PA_START);
	CHECK(pg2va(PAGES + 0) == PGFRAME_VA_START);
	CHECK(pg2pa(PAGES + 1) == PGFRAME_PA_START + 1*PGFRAME_PGSIZE);
	CHECK(pg2va(PAGES + 1) == PGFRAME_VA_START + 1*PGFRAME_PGSIZE);
	CHECK(pg2pa(PAGES + 3) == PGFRAME_PA_START + 3*PGFRAME_PGSIZE);
	CHECK(pg2va(PAGES + 3) == PGFRAME_VA_START + 3*PGFRAME_PGSIZE);
	CHECK(pg2pa(PAGES + 123) == PGFRAME_PA_START + 123*PGFRAME_PGSIZE);
	CHECK(pg2va(PAGES + 123) == PGFRAME_VA_START + 123*PGFRAME_PGSIZE);
	CHECK(pa_valid(PGFRAME_PA_START) == 1);
	CHECK(pa_valid(PGFRAME_PA_END) == 0);
	CHECK(pa_valid(PGFRAME_PA_END -1) == 1);
	CHECK(pa_valid(PGFRAME_PA_START -1) == 0);
	CHECK(va_valid(PGFRAME_VA_START -1) == 0);
	CHECK(va_valid(PGFRAME_VA_START) == 1);
	CHECK(va_valid(PGFRAME_VA_END) == 0);
	CHECK(va_valid(PGFRAME_VA_END -1) == 1);
	CHECK(id_valid(0) == 1);
	CHECK(id_valid(0 -1) == 0);
	CHECK(id_valid(PGFRAME_PAGE_NUMS) == 0);
	CHECK(id_valid(PGFRAME_PAGE_NUMS -1) == 1);
	CHECK(pg_valid(PAGES) == 1);
	CHECK(pg_valid(PAGES -1) == 0);
	CHECK(pg_valid(PAGES + PGFRAME_PAGE_NUMS) == 0);
	CHECK(pg_valid(PAGES + PGFRAME_PAGE_NUMS -1) == 1);

	return 0;
}
