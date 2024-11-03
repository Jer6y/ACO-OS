#ifndef __ACO_MM_BUDDY_H
#define __ACO_MM_BUDDY_H

#include <stdint.h>
#include <generated/autoconf.h>
#include <aco/spinlock.h>
#include <aco/list.h>
#include <mm/pageframe.h>

#define  MAX_ORDER 		((int)(CONFIG_BUDDYSYSTEM_MAX_ORDER))
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

extern 	buddy_allocator_t 	g_buddy;
#define GLOBAL_BUDDY_ALOCTR	g_buddy


#define buddy_pgfm_freeable(pgfm)	(((pgfm)->meta).buddy_flags == PG_BUDDY_FLAG_HEAD)
#define buddy_pgfm_infrlist(pgfm)	(((pgfm)->meta).buddy_flags == (PG_BUDDY_FLAG_HEAD | PG_BUDDY_FLAG_FREE))

pageframe_t* bkp_alloc(int order);

void 	bkp_free(struct pageframe* pgfm);	

void* 	bk_alloc(int order);

void  	bk_free(void* addres);

int  	buddy_slfcheck(void);

#endif /* __ACO_MM_BUDDY_H */
