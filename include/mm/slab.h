#ifndef __ACO_SLAB_H
#define __ACO_SLAB_H

#include <aco/list.h>
#include <aco/spinlock.h>

typedef struct kmem_cache {
	spinlock_t lk;
	int  obj_num;       	// cached object nums
	int  obj_size;  	// cached object size
	int  pg_order;		// all pageframe's page_order (those pageframe hanged on the free/full/partial list)
	int  pg_orlimit;	// max pageframe to store in the kmem_cache
	int  pg_orhas;		
	int  color_offset;
	struct list_head  free;
	struct list_head  full;
	struct list_head  partial;
} kmem_cache_t;

extern  kmem_cache_t 	slab_kmm_cache;
#define SLAB_CACHE	slab_kmm_cache

kmem_cache_t* 	kmem_cache_alloc();
void		kmem_cache_free(kmem_cache_t* kmm_cache);
int		kmem_cache_init(kmem_cache_t* kmm_cache, int obj_size, int pg_order, int pgor_limit, int init_pgor_alloc);
void 		kmem_cache_deinit(kmem_cache_t* kmm_cache);
void*		kmem_obj_alloc(kmem_cache_t* kmm_cache);
void		kmem_obj_free(kmem_cache_t* kmm_cache, void* address);

typedef struct kmem_cache_info {
	int  obj_num;
	int  obj_size;
	int  pg_orlimit;
	int  pg_orhas;
	int  pg_order;
} kmm_cinfo;
void		kmem_cached_info(kmem_cache_t* kmm_cache, kmm_cinfo* info);

#endif /* __ACO_SLAB_H */
