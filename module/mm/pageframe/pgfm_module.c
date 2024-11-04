#include <mm/pageframe.h>
#include <aco/errno.h>
#include <aco/log.h>
#include <aco/init.h>
#include <aco/string.h>

DEFINE_ATOMIC(pgfm_module_init_done,0);
struct pageframe* pgframe_pages;

#define pgfm_error(reason, info) 	do 	\
					{	\
						log_error("[PGFRAME] : " reason "!");\
						log_error("[PGFRAME] : " info   "!");  \
					} while(0)


FUNC_BUILTIN int pgfm_module_init(void)
{
	if(atomic_fetch_and_add(&pgfm_module_init_done,0) != 0)
		return 0;
	pgframe_pages = NULL;
	if((uintptr_t)PGFRAME_PA_START > (uintptr_t)PGFRAME_VA_START)
	{
		pgfm_error("Arch Implementation Error", "PA_START > VA_START");
		log_error("[PGFRAME] : PA_START 0x%p",PGFRAME_PA_START);
		log_error("[PGFRAME] : VA_START 0x%p", PGFRAME_VA_START);
		return -EFAULT;
	}
	if(PA_PAGE_ALIGNED(PGFRAME_PA_START) ==0)
	{
		pgfm_error("Arch Implementation Error", "PA_START NO ALIGNED");
		log_error("[PGFRAME] : PA_START 0x%p",PGFRAME_PA_START);
		return -EFAULT;
	}	
	if(VA_PAGE_ALIGNED(PGFRAME_VA_START) ==0)
	{
		pgfm_error("Arch Implementation Error", "VA_START NO ALIGNED");
		log_error("[PGFRAME] : VA_START 0x%p",PGFRAME_VA_START);
		return -EFAULT;
	}
	if(VAL_IS_PAGE_ALIGNED(PGFRAME_SIZE) ==0)
	{
		pgfm_error("Arch Implementation Error", "PGF_SIZE NO ALIGNED");
		log_error("[PGFRAME] : PGF_SIZE 0x%p",PGFRAME_VA_START);
		return -EFAULT;
	}

	if((uintptr_t)PGFRAME_SIZE <= 0)
	{
		pgfm_error("Arch Implementation Error", "PGF_SIZE IS ZERO!!!");
		log_error("[PGFRAME] : PGF_SIZE 0x%p",PGFRAME_VA_START);
		return -EFAULT;
	}
	uintptr_t kernel_start = PGFRAME_PA_KSTART;
	uintptr_t kernel_end   = PGFRAME_PA_KSTART + PGFRAME_K_SIZE;
	if(!(kernel_start >= (uintptr_t)PGFRAME_PA_START && kernel_start < (uintptr_t)PGFRAME_PA_END\
			&& kernel_end >  (uintptr_t)PGFRAME_PA_START \
			&& kernel_end <= (uintptr_t)PGFRAME_PA_END))
	{
		pgfm_error("Arch Implementation Error", "KERNEL NOT IN MEMRY");
		log_error("[PGFRAME] : PA_KSTRT 0x%p",kernel_start);
		log_error("[PGFRAME] : PA_KSTRT 0x%p",kernel_start);
		log_error("[PGFRAME] : PA_MSTRT 0x%p",PGFRAME_PA_START);
		log_error("[PGFRAME] : PA_MEND  0x%p",PGFRAME_PA_END);
		return -EFAULT;
	}
	uintptr_t free_start = PGFRAME_FREE_VA_START;
	uintptr_t free_end = PGFRAME_FREE_VA_START +  PGFRAME_FREE_SIZE;
	if(!(free_start >= (uintptr_t)PGFRAME_VA_START \
			&& free_start < (uintptr_t)PGFRAME_VA_END \
			&& free_end >  (uintptr_t)PGFRAME_VA_START\
			&& free_end <= (uintptr_t)PGFRAME_VA_END))
	{
		pgfm_error("Arch Implementation Error", "FREEAREA NOT IN MEM");
		log_error("[PGFRAME] : VA_FRSRT 0x%p",free_start);
		log_error("[PGFRAME] : VA_FREND 0x%p",free_end);
		log_error("[PGFRAME] : VA_MRSRT 0x%p",PGFRAME_VA_START);
		log_error("[PGFRAME] : VA_MREND 0x%p",PGFRAME_VA_END);
		return -EFAULT;
	}
	int need_alloc_size = PGFRAME_PAGE_NUMS * sizeof(struct pageframe);
	if(need_alloc_size > PGFRAME_FREE_SIZE)
	{
		pgfm_error("Memory Not Enough Error", "NEED LESS THAN FREE");
		log_error("[PGFRAME] : NEED MEM 0x%p",need_alloc_size);
		log_error("[PGFRAME] : FREE MEM 0x%p",PGFRAME_FREE_SIZE);
		return -ENOMEM;
	}
	pgframe_pages = (struct pageframe*)free_start;
	free_start += need_alloc_size;
	free_start = PAGE_UPPER_ALIGN(free_start);
	free_end   = PAGE_DOWNS_ALIGN(free_end);
	if(free_start >= free_end)
	{
		pgfm_error("Rest Memory Not Enough Error", "REST MEMORY IS  ZERO");
		pgframe_pages = NULL;
		return -ENOMEM;
	}
	for(int i=0;i<(int)PGFRAME_PAGE_NUMS;i++)
	{
		init_lock(&(pgframe_pages[i].lk));
		pgframe_pages[i].pgtype = PAGE_BUDDYALLOCATOR;
		memset(&(pgframe_pages[i].meta_other), 0, sizeof(pgframe_pages[i].meta_other));
		if( i == 0)
		{
			log_debug("[PGFRAME] : set meta othr sz :0x%p", sizeof(pgframe_pages[i].meta_other));
			pgframe_pages[i].meta_other.ref_count = 0;
			pgframe_pages[i].meta_other.vma_flags = 0;
			log_debug("[PGFRAME] : wrt unalgned tst :1/1", sizeof(pgframe_pages[i].meta_other));

		}
		INIT_NODE(&(pgframe_pages[i].meta.buddy_node));
		pgframe_pages[i].meta.buddy_order = 0;
		if(pg2va(pgframe_pages + i) >= free_start && pg2va(pgframe_pages + i) < free_end)
		{
			pgframe_pages[i].meta.buddy_flags = PG_BUDDY_FLAG_HEAD;
		}
		else
		{
			pgframe_pages[i].meta.buddy_flags = PG_BUDDY_FLAG_STATIC;
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
	atomic_fetch_and_add(&pgfm_module_init_done,1);
	return 0;
}

FUNC_BUILTIN void pgfm_module_exit(void)
{
	return;
}

REGISTER_MODULE_INIT(PRIO_1, pgfm_module_init);
REGISTER_MODULE_EXIT(PRIO_1, pgfm_module_exit);
