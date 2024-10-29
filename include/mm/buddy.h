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
	spinlock_t	 	lk;
	int			rest_block_num;
	int			block_size;
	struct list_head 	_list;
} mempool_alloctor_t;

typedef struct buddy_allocator {
	mempool_alloctor_t order_pools[MAX_ORDER];
} buddy_allocator_t;

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


#endif /* __ACO_MM_BUDDY_H */
