#include <aco/types.h>
#include <mm/pageframe.h>
#include <aco/errno.h>
#include <mm/page.h>
#include <aco/log.h>

struct pageframe* pages;

int pageframe_init(void)
{
	pages = NULL;
	if((uintptr_t)PGFRAME_PA_START > (uintptr_t)PGFRAME_VA_START)
		return -EFAULT;
	if(PA_PAGE_ALIGNED(PGFRAME_PA_START) ==0 || VA_PAGE_ALIGNED(PGFRAME_VA_START) ==0 \
			|| VAL_IS_PAGE_ALIGNED(PGFRAME_SIZE) ==0 )
		return -EFAULT;
	if((uintptr_t)PGFRAME_SIZE <= 0)
		return -EFAULT;
	uintptr_t kernel_start = PGFRAME_PA_KSTART;
	uintptr_t kernel_end   = PGFRAME_PA_KSTART + PGFRAME_K_SIZE;
	if(!(kernel_start >= (uintptr_t)PGFRAME_PA_START && kernel_start < (uintptr_t)PGFRAME_PA_END\
			&& kernel_end >  (uintptr_t)PGFRAME_PA_START \
			&& kernel_end <= (uintptr_t)PGFRAME_PA_END))
		return -EFAULT;
	uintptr_t free_start = PGFRAME_FREE_VA_START;
	uintptr_t free_end = PGFRAME_FREE_VA_START +  PGFRAME_FREE_SIZE;
	if(!(free_start >= (uintptr_t)PGFRAME_VA_START \
			&& free_start < (uintptr_t)PGFRAME_VA_END \
			&& free_end >  (uintptr_t)PGFRAME_VA_START\
			&& free_end <= (uintptr_t)PGFRAME_VA_END))
		return -EFAULT;
	int need_alloc_size = PGFRAME_PAGE_NUMS * sizeof(struct pageframe);
	if(need_alloc_size > PGFRAME_FREE_SIZE)
		return -ENOMEM;
	pages = (struct pageframe*)free_start;
	free_start += need_alloc_size;
	free_start = PAGE_UPPER_ALIGN(free_start);
	free_end   = PAGE_DOWNS_ALIGN(free_end);
	if(free_start >= free_end)
	{
		pages = NULL;
		return -ENOMEM;
	}
	for(int i=0;i<(int)PGFRAME_PAGE_NUMS;i++)
	{
		init_lock(&(pages[i].lk));
		pages[i].pgtype = PAGE_BUDDYALLOCATOR;
	        	
		INIT_NODE(&(pages[i].meta.buddy_node));
		pages[i].meta.buddy_order = 0;
		if(pg2va(pages + i) >= free_start && pg2va(pages + i) < free_end)
		{
			pages[i].meta.buddy_flags = PG_BUDDY_FLAG_HEAD;
		}
		else
		{
			pages[i].meta.buddy_flags = PG_BUDDY_FLAG_STATIC;
		}
	}
	log_debug("[PGFRAME] : memory_pa start : 0x%p",PGFRAME_PA_START);
	log_debug("[PGFRAME] : memory_pa end   : 0x%p",PGFRAME_PA_END);
	log_debug("[PGFRAME] : memory_va start : 0x%p",PGFRAME_VA_START);
	log_debug("[PGFRAME] : memory_va end   : 0x%p",PGFRAME_VA_END);
	log_debug("[PGFRAME] : memory_pa size  : 0x%x",PGFRAME_SIZE);
	log_debug("[PGFRAME] : memory num page : 0x%x",PGFRAME_PAGE_NUMS);
	log_debug("[PGFRAME] : freemm_va start : 0x%p",free_start);
	log_debug("[PGFRAME] : freemm_va end   : 0x%p",free_end);
	log_debug("[PGFRAME] : freemm tota size: 0x%x",free_end - free_start);
	log_debug("[PGFRAME] : all pages init done!");
	return 0;
}
