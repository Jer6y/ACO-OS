#include <mm/buddy.h>
#include <mm/pageframe.h>
#include <aco/assert.h>
#include <aco/log.h>
static buddy_allocator_t g_buddy;


#define buddy_pgfm_freeable(pgfm) (((pgfm)->meta).buddy_flags == PG_BUDDY_FLAG_HEAD)

/*
static int buddy_pgfm_static(struct pageframe* pgfm)
{
        return ( (((pgfm->meta).buddy_flags & PG_BUDDY_FLAG_STATIC) != 0)\
                        && (((pgfm->meta).buddy_flags & PG_BUDDY_FLAG_FREE)==0)\
                        && (((pgfm->meta).buddy_flags & PG_BUDDY_FLAG_HEAD) == 0)\
                        && (((pgfm->meta).buddy_flags & PG_BUDDY_FLAG_BODY) == 0));
}
*/


// call it after pageframe-init done
int buddy_init(void)
{
	for(int i=0;i<MAX_ORDER;i++)
	{
		init_lock(&((g_buddy.order_pools)[i].lk));
		(g_buddy.order_pools)[i].rest_block_num = 0;
		(g_buddy.order_pools)[i].block_size = ORDER2BYTE(i);
		INIT_LIST_HEAD(&((g_buddy.order_pools)[i]._list));
	}
	for(int i =0;i<PGFRAME_PAGE_NUMS;i++)
	{
		bkp_free(PAGES + i);
	}
	log_debug("[BUDDY  ]: MAXORDER %d",MAX_ORDER);
	for(int i=0;i<MAX_ORDER;i++)
	{
		log_debug("[BUDDY %d]: blk_size 0x%x",i,g_buddy.order_pools[i].block_size);
		log_debug("[BUDDY %d]: rst_blks 0x%x",i,g_buddy.order_pools[i].rest_block_num);
	}
	return 0;
}

// buddy kernel pageframe alloc
struct pageframe* bkp_alloc(int order_orig)
{
	if(!(order_orig >=0 && order_orig < MAX_ORDER))
		return NULL;
	int order = order_orig;
	while(order < MAX_ORDER)
	{
		lock(&(g_buddy.order_pools[order].lk));
		if(g_buddy.order_pools[order].rest_block_num ==0)
		{
			unlock(&(g_buddy.order_pools[order].lk));
			order++;
			continue;
		}
		g_buddy.order_pools[order].rest_block_num--;
		pageframe_t* page_ptr = list_first_entry(&(g_buddy.order_pools[order]._list),pageframe_t,meta.buddy_node);
                lock(&(page_ptr->lk));
                list_del(&((page_ptr->meta).buddy_node));
                ASSERT((page_ptr->meta).buddy_flags == (PG_BUDDY_FLAG_HEAD |PG_BUDDY_FLAG_FREE));
                ASSERT((page_ptr->meta).buddy_order == order);
                (page_ptr->meta).buddy_flags = PG_BUDDY_FLAG_HEAD;
		(page_ptr->meta).buddy_order = order_orig;
		unlock(&(page_ptr->lk));
		unlock(&(g_buddy.order_pools[order].lk));
		pageframe_t* rest_pgst = (page_ptr + ORDER2PGNUM(order_orig));
		int rest_pgnum = ORDER2PGNUM(order) - ORDER2PGNUM(order_orig);
		for(int i=MAX_ORDER-1;i>=0;i--)
		{
			if(rest_pgnum == 0)
				break;
			if((rest_pgnum & (1<<i)) != 0)
			{
				lock(&(rest_pgst)->lk);
				ASSERT((rest_pgst->meta).buddy_flags == (PG_BUDDY_FLAG_BODY));
				ASSERT((rest_pgst->meta).buddy_order == MAX_ORDER);
				(rest_pgst->meta).buddy_order = i;
				(rest_pgst->meta).buddy_flags = PG_BUDDY_FLAG_HEAD;
				unlock(&(rest_pgst)->lk);
				bkp_free(rest_pgst);
				rest_pgst  += (1<<i);
				rest_pgnum -= (1<<i);
			}
		}
		return page_ptr;
	}
	return NULL;
}

// buddy kernel pageframe free
void bkp_free(struct pageframe* pgfm)
{
	if(pgfm == NULL)
		return;
	if(!pg_valid(pgfm))
		return;
	lock(&(pgfm->lk));
	if(pgfm->pgtype != PAGE_BUDDYALLOCATOR)
	{
		unlock(&(pgfm->lk));
                return;
	}
	if(!buddy_pgfm_freeable(pgfm))
	{
		unlock(&(pgfm->lk));
                return;
	}
	int order = (pgfm->meta).buddy_order;
	unlock(&(pgfm->lk)); 
	// here is dangerous 
	// what will happen if other thread gain the pgfm->lk 
	// and  change its order or buddy_flags or even pgfm_type
	// but we need to do priority lock sequence to prevent deadlock
	while(order < MAX_ORDER -1)
	{
		lock(&((g_buddy.order_pools)[order].lk));
		lock(&(pgfm->lk));
		ASSERT(pgfm->pgtype == PAGE_BUDDYALLOCATOR \
				&& (pgfm->meta).buddy_order == order\
				&& (pgfm->meta).buddy_flags == PG_BUDDY_FLAG_HEAD);
		viraddr_t bef_pgva = pg2va(pgfm) - ORDER2BYTE(order);
		if(va_valid(bef_pgva))
		{
			struct pageframe* bef_page = va2pg(bef_pgva);
			lock(&(bef_page->lk));
			if(bef_page->pgtype == PAGE_BUDDYALLOCATOR \
					&& ((bef_page->meta).buddy_order == order)\
					&& (((bef_page->meta).buddy_flags & PG_BUDDY_FLAG_HEAD) !=0)\
					&& (((bef_page->meta).buddy_flags & PG_BUDDY_FLAG_BODY) ==0)\
					&& (((bef_page->meta).buddy_flags & PG_BUDDY_FLAG_FREE) !=0))
			{
				list_del(&((bef_page->meta).buddy_node));
				g_buddy.order_pools[order].rest_block_num --;
				(pgfm->meta).buddy_order = MAX_ORDER;
				(pgfm->meta).buddy_flags = PG_BUDDY_FLAG_BODY;
				(bef_page->meta).buddy_order += 1;
				(bef_page->meta).buddy_flags = PG_BUDDY_FLAG_HEAD;
				unlock(&(bef_page->lk));
				unlock(&(pgfm->lk));
				unlock(&((g_buddy.order_pools)[order].lk));
				order++;
				pgfm = bef_page;
				continue;
			}
			unlock(&(bef_page->lk));
		}
		viraddr_t aft_pgva = pg2va(pgfm) + ORDER2BYTE(order);
		if(va_valid(aft_pgva))
		{
			struct pageframe* aft_page = va2pg(aft_pgva);
			lock(&(aft_page->lk));
			if(aft_page->pgtype == PAGE_BUDDYALLOCATOR \
					&& (((aft_page->meta).buddy_order) == order)\
					&& (((aft_page->meta).buddy_flags & PG_BUDDY_FLAG_HEAD) !=0 )\
					&& (((aft_page->meta).buddy_flags & PG_BUDDY_FLAG_BODY) ==0 )\
					&& (((aft_page->meta).buddy_flags & PG_BUDDY_FLAG_FREE) !=0 ))
			{
				list_del(&((aft_page->meta).buddy_node));
				g_buddy.order_pools[order].rest_block_num --;
				(pgfm->meta).buddy_order += 1;
				(pgfm->meta).buddy_flags = PG_BUDDY_FLAG_HEAD;
				(aft_page->meta).buddy_order = MAX_ORDER;
				(aft_page->meta).buddy_flags = PG_BUDDY_FLAG_BODY;
				unlock(&(aft_page->lk));
				unlock(&(pgfm->lk));
                                unlock(&((g_buddy.order_pools)[order].lk));
                                order++;
				continue;
			}
			unlock(&(aft_page->lk));
		}
		unlock(&(pgfm->lk));
                unlock(&((g_buddy.order_pools)[order].lk));
		break;
	}
	lock(&((g_buddy.order_pools)[order].lk));
        lock(&(pgfm->lk));
	(pgfm->meta).buddy_flags |= PG_BUDDY_FLAG_FREE;
	ASSERT(pgfm->pgtype == PAGE_BUDDYALLOCATOR\
			&& (((pgfm->meta).buddy_flags & PG_BUDDY_FLAG_FREE )!=0 )\
			&& (((pgfm->meta).buddy_flags & PG_BUDDY_FLAG_HEAD )!=0 )\
			&& (((pgfm->meta).buddy_flags & PG_BUDDY_FLAG_BODY )==0 )\
			&& (((pgfm->meta).buddy_flags & PG_BUDDY_FLAG_STATIC )==0 ));
	list_add_tail(&((pgfm->meta).buddy_node),&(g_buddy.order_pools[order]._list));
	g_buddy.order_pools[order].rest_block_num ++;
	unlock(&(pgfm->lk));
	unlock(&((g_buddy.order_pools)[order].lk));
	return;
}
// buddy kernel alloc
void* bk_alloc(int order)
{
	pageframe_t* pgfm = bkp_alloc(order);
	if(pgfm == NULL)
		return NULL;
	return (void*)pg2va(pgfm);
}

// buddy kernel free
void  bk_free(void* addres)
{
	if(addres == NULL || !va_valid(addres))
		return;
	pageframe_t* pgfm = va2pg((viraddr_t)addres);
	bkp_free(pgfm);
	return;
}

#include <mm/page.h>
#include <aco/errno.h>
int  buddy_slfcheck(void)
{
	for(int i=0;i<MAX_ORDER;i++)
	{
		lock(&(g_buddy.order_pools[i].lk));
		if(g_buddy.order_pools[i].block_size != (1<<i)*PAGE_SIZE)
		{
			unlock(&(g_buddy.order_pools[i].lk));
			return -EFAULT;
		}
		int rest_block = g_buddy.order_pools[i].rest_block_num;
		struct list_head* iterator;
		int err =0;
		list_for_each(iterator,&(g_buddy.order_pools[i]._list))
		{
			pageframe_t* pg = list_entry(iterator, pageframe_t, meta.buddy_node);
			if((pg->meta).buddy_order != i)
				err =1;
			rest_block--;
		}
		if(rest_block !=0 || err ==1)
		{
			unlock(&(g_buddy.order_pools[i].lk));
			return -EFAULT;
		}
		unlock(&(g_buddy.order_pools[i].lk));
	}
	return 0;
}

/*
void buddy_information(void)
{
	log_debug("[BUDDY  ]: MAXORDER %d",MAX_ORDER);
        for(int i=0;i<MAX_ORDER;i++)
        {
		lock(&(g_buddy.order_pools[i].lk));
                log_debug("[BUDDY %d]: blk_size 0x%x",i,g_buddy.order_pools[i].block_size);
                log_debug("[BUDDY %d]: rst_blks 0x%x",i,g_buddy.order_pools[i].rest_block_num);
        	unlock(&(g_buddy.order_pools[i].lk));
	}
}
*/
