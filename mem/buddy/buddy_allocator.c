#include <mm/buddy.h>
#include <mm/pageframe.h>
#include <aco/assert.h>

static buddy_allocator_t g_buddy;


static int buddy_pgfm_freeable(struct pageframe* pgfm)
{

        return (((((pgfm->meta).buddy_flags) & PG_BUDDY_FLAG_FREE) ==0)\
                        && (((pgfm->meta).buddy_flags & PG_BUDDY_FLAG_STATIC) == 0)\
                        && (((pgfm->meta).buddy_flags & PG_BUDDY_FLAG_HEAD) != 0)\
                        && (((pgfm->meta).buddy_flags & PG_BUDDY_FLAG_BODY) == 0));
}

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
	return 0;
}

// buddy kernel pageframe alloc
struct pageframe* bkp_alloc(int order)
{
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
	(pgfm->meta).buddy_flags = (PG_BUDDY_FLAG_FREE | PG_BUDDY_FLAG_HEAD);
	unlock(&(pgfm->lk));
	while(order < MAX_ORDER -1)
	{
		lock(&((g_buddy.order_pools)[order].lk));
		lock(&(pgfm->lk));
		viraddr_t bef_pgva = pg2va(pgfm) - ORDER2BYTE(order);
		if(va_valid(bef_pgva))
		{
			struct pageframe* bef_page = va2pg(bef_pgva);
			lock(&(bef_page->lk));
			if(bef_page->pgtype == PAGE_BUDDYALLOCATOR \
					&& ((bef_page->meta).buddy_order == order)\
					&& (((bef_page->meta).buddy_flags & PG_BUDDY_FLAG_FREE) !=0)\
					&& (((bef_page->meta).buddy_flags & PG_BUDDY_FLAG_HEAD) !=0)\
					&& (((bef_page->meta).buddy_flags & PG_BUDDY_FLAG_BODY) ==0))
			{
				list_del(&((bef_page->meta).buddy_node));
				g_buddy.order_pools[order].rest_block_num --;
				(pgfm->meta).buddy_order += 1;
				(pgfm->meta).buddy_flags = (PG_BUDDY_FLAG_FREE | PG_BUDDY_FLAG_BODY);
				(bef_page->meta).buddy_order += 1;
				(bef_page->meta).buddy_flags = (PG_BUDDY_FLAG_FREE | PG_BUDDY_FLAG_HEAD);
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
					&& (((aft_page->meta).buddy_flags & PG_BUDDY_FLAG_FREE) !=0 )\
					&& (((aft_page->meta).buddy_flags & PG_BUDDY_FLAG_HEAD) !=0 )\
					&& (((aft_page->meta).buddy_flags & PG_BUDDY_FLAG_BODY) ==0 ))
			{
				list_del(&((aft_page->meta).buddy_node));
				g_buddy.order_pools[order].rest_block_num --;
				(pgfm->meta).buddy_order += 1;
				(pgfm->meta).buddy_flags = (PG_BUDDY_FLAG_FREE | PG_BUDDY_FLAG_HEAD);
				(aft_page->meta).buddy_order += 1;
				(aft_page->meta).buddy_flags = (PG_BUDDY_FLAG_FREE | PG_BUDDY_FLAG_BODY);
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
	return NULL;
}

// buddy kernel free
void  bk_free(void* addres)
{
	return;
}

