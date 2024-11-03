#include <mm/buddy.h>
#include <aco/init.h>
#include <aco/log.h>

DEFINE_ATOMIC(buddy_module_init_done,0);
buddy_allocator_t g_buddy;

FUNC_BUILTIN int buddy_module_init(void)
{
	if(atomic_fetch_and_add(&buddy_module_init_done,0) != 0)
		return 0;
	for(int i=0;i<MAX_ORDER;i++)
        {
                init_lock(&((g_buddy.order_pools)[i].lk));
                (g_buddy.order_pools)[i].rest_block_num = 0;
                (g_buddy.order_pools)[i].block_size = ORDER2BYTE(i);
                INIT_LIST_HEAD(&((g_buddy.order_pools)[i]._list));
        }
        for(int i =0;i<PGFRAME_PAGE_NUMS;i++)
        {
                if(buddy_pgfm_freeable(&(PAGES[i])))
                        bkp_free(PAGES + i);
        }
        log_debug("[BUDDY  ]: MAXORDER %d",MAX_ORDER);
        for(int i=0;i<MAX_ORDER;i++)
        {
                log_debug("[BUDDY %d]: blk_size 0x%x",i,g_buddy.order_pools[i].block_size);
                log_debug("[BUDDY %d]: rst_blks 0x%x",i,g_buddy.order_pools[i].rest_block_num);
        }
	atomic_fetch_and_add(&buddy_module_init_done,1);
        return 0;
}

FUNC_BUILTIN void buddy_module_exit(void)
{
	return;
}


REGISTER_MODULE_INIT(PRIO_2, buddy_module_init);
REGISTER_MODULE_EXIT(PRIO_2, buddy_module_exit);
