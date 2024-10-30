#ifndef __ACO_MM_BUDDY_H
#define __ACO_MM_BUDDY_H

#include <aco/types.h>
#include <aco/spinlock.h>
#include <aco/list.h>
#include <generated/autoconf.h>
#include <mm/pageframe.h>

#define  MAX_ORDER 		CONFIG_BUDDYSYSTEM_MAX_ORDER

#define  ORDER2PGNUM(order)	(1ULL<<(order))
#define  ORDER2BYTE(order)	((size_t)ORDER2PGNUM(order) * PGFRAME_PGSIZE)

typedef struct mempool_allocator {
	spinlock_t              lk;	
	int			rest_block_num;
	int			block_size;
	struct list_head 	_list;
} mempool_alloctor_t;

typedef struct buddy_allocator {
	mempool_alloctor_t order_pools[MAX_ORDER];
} buddy_allocator_t;

/* without lock and no check for parameter */
static inline bool pgfm_is_body(pageframe_t* pgfm)
{
	return (((pgfm->meta).buddy_flags & PG_BUDDY_FLAG_BODY) != 0);
}

/* without lock and no check for parameter */
static inline bool pgfm_is_head(pageframe_t* pgfm)
{
	return (((pgfm->meta).buddy_flags & PG_BUDDY_FLAG_HEAD) != 0);
}

/* without lock and no check for parameter */
static inline void pgfm_set_ashead(pageframe_t* pgfm)
{
	(pgfm->meta).buddy_flags &= (~PG_BUDDY_FLAG_BODY);
	(pgfm->meta).buddy_flags |= PG_BUDDY_FLAG_HEAD;
}

/* without lock and no check for parameter */
static inline void pgfm_set_asbody(pageframe_t* pgfm)
{
        (pgfm->meta).buddy_flags |= PG_BUDDY_FLAG_BODY;
        (pgfm->meta).buddy_flags &= (~PG_BUDDY_FLAG_HEAD);
}

/* without lock and no check for parameter */
static inline void pgfm_set_asfree(pageframe_t* pgfm)
{
	(pgfm->meta).buddy_flags |= PG_BUDDY_FLAG_FREE;
}

/* without lock and no check for parameter */
static inline void pgfm_set_asused(pageframe_t* pgfm)
{
	(pgfm->meta).buddy_flags &= (~PG_BUDDY_FLAG_FREE);
}

/* without lock and no check for parameter */
static inline void pgfm_set_asfreehead(pageframe_t* pgfm)
{
	pgfm_set_asfree(pgfm);
	pgfm_set_ashead(pgfm);
}

/* without lock and no check for parameter */
static inline void pgfm_set_asusedhead(pageframe_t* pgfm)
{
        pgfm_set_asused(pgfm);
        pgfm_set_ashead(pgfm);
}


/* without lock and no check for parameter */
static inline void pgfm_merge(pageframe_t* merger, pageframe_t* mergee)
{
	(merger->meta).buddy_order++;
	(mergee->meta).buddy_order  = MAX_ORDER;
	(merger->meta).buddy_flags &= (~PG_BUDDY_FLAG_BODY);
	(merger->meta).buddy_flags |= PG_BUDDY_FLAG_HEAD;
	(mergee->meta).buddy_flags |= PG_BUDDY_FLAG_BODY;
        (mergee->meta).buddy_flags &= (~PG_BUDDY_FLAG_HEAD);
}

// call it after pageframe-init done
int buddy_init(void);

// buddy kernel pageframe alloc
struct pageframe* bkp_alloc(int order);

// buddy kernel pageframe free 
void bkp_free(struct pageframe* pgfm);	

// buddy kernel alloc
void* bk_alloc(int order);

// buddy kernel free
void  bk_free(void* addres);

// buddy self check
// if error , will return a value <0;
int  buddy_slfcheck(void);

#endif /* __ACO_MM_BUDDY_H */
