#include <mm/slab.h>
#include <mm/pageframe.h>
#include <aco/errno.h>
#include <aco/assert.h>
#include <mm/page.h>
#include <aco/math.h>
#include <mm/buddy.h>
#include <aco/string.h>
#include <aco/log.h>

typedef struct slab_s {
        int      obj_all;
        int      obj_free;
	int      obj_size;
        int      mem_free_head;
        int      mem_free_tail;
        int*     mem_index_start;
        void*    mem_obj_start;
} slab_t;

//Note : mem_offset is passed in to decide how obj_start offset will be
//becuase we want to do color slab as Linux do
static inline slab_t* init_slab_xip(void* start, int size, int obj_size, int offset)
{
	ASSERT(start != NULL && size >0 \
			&& obj_size >0 && size > obj_size\
			&& (VAL_IS_PAGE_ALIGNED(start) == 1));
	slab_t* slab_handler = (slab_t*)start;
	int obj_num =  (size - sizeof(slab_t))/(sizeof(int)+obj_size);
	int padding =  (size - sizeof(slab_t))%(sizeof(int)+obj_size);
	if(padding != 0)
	{
		offset = SDBMHash((char*)(&offset),sizeof(int));	
		offset %= padding;
		offset &= (~(0b11)); 
		// do color
	}
	else
		offset = 0;
	ASSERT(offset >=0);
	if(obj_num <= 0)
		return NULL;
	slab_handler->obj_all = obj_num;
	slab_handler->obj_free = obj_num;
	slab_handler->obj_size = obj_size;
	slab_handler->mem_index_start = (int*)(((char*)start)+sizeof(slab_t));
	slab_handler->mem_obj_start = (void*)(((uintptr_t)((slab_handler->mem_index_start)+obj_num)) + offset);
	for(int i=0;i<obj_num;i++)
	{
		if(i != obj_num-1)
			(slab_handler->mem_index_start)[i] = i+1;
        	else
            		(slab_handler->mem_index_start)[i] = i;
	}
	slab_handler->mem_free_head = 0;
	slab_handler->mem_free_tail = obj_num-1;
	return slab_handler;
}

static inline void* slab_alloc(slab_t* slab_handler)
{
	ASSERT(slab_handler != NULL);
	void* ret;
	if(slab_handler->obj_free == 0)
		return NULL;
	int index = slab_handler->mem_free_head;
	ASSERT(index != -1);
	ret = (void*)((char*)(slab_handler->mem_obj_start) + (slab_handler->obj_size)*index);
	slab_handler->mem_free_head = slab_handler->mem_index_start[index];
	slab_handler->mem_index_start[index] = 0xffffffff;
	slab_handler->obj_free--;
	if(slab_handler->obj_free ==0)
	{
        	slab_handler->mem_free_head = -1;
        	slab_handler->mem_free_tail = -1;
    	}
	return ret;
}

static inline void slab_free(slab_t* slab_handler, void* address)
{
	ASSERT(slab_handler != NULL && address != NULL);
	ASSERT(((char*)address >= (char*)(slab_handler->mem_obj_start)) \
			&& ((char*)address < (char*)(slab_handler->mem_obj_start + (slab_handler->obj_all) * (slab_handler->obj_size))));
	int index = (int)((char*)address - (char*)(slab_handler->mem_obj_start));
	ASSERT(index % slab_handler->obj_size == 0);
	index /= slab_handler->obj_size;
	ASSERT((slab_handler->mem_index_start)[index] == 0xffffffff);
	if(slab_handler->obj_free ==0)
	{
		ASSERT(slab_handler->mem_free_head ==-1 && slab_handler->mem_free_tail == -1);
		slab_handler->mem_free_head = index;
		slab_handler->mem_free_tail = index;
		(slab_handler->mem_index_start)[index] = index;
	}
	else
	{
		ASSERT(slab_handler->mem_free_head !=-1 && slab_handler->mem_free_tail != -1);
		(slab_handler->mem_index_start)[slab_handler->mem_free_tail] = index;
		(slab_handler->mem_index_start)[index] = index;
		slab_handler->mem_free_tail = index;
	}
	slab_handler->obj_free++;
	return;
}



static inline void set_pgfm_aslab(pageframe_t* pgfm)
{
	ASSERT(pgfm != NULL 				\
	       && pgfm->pgtype == PAGE_BUDDYALLOCATOR	\
	       && (pgfm->meta).buddy_flags == PG_BUDDY_FLAG_HEAD);
        lock(&(pgfm->lk));
        pgfm->pgtype = SLAB_ALLOCATOR;
        unlock(&(pgfm->lk));
	return;
}

static inline void set_pgfm_asbuddy(pageframe_t* pgfm)
{
	ASSERT(pgfm != NULL                             \
               && pgfm->pgtype == SLAB_ALLOCATOR   	\
               && (pgfm->meta).buddy_flags == PG_BUDDY_FLAG_HEAD);
	lock(&(pgfm->lk));
	pgfm->pgtype = PAGE_BUDDYALLOCATOR;
	unlock(&(pgfm->lk));
	return;
}

static inline slab_t* init_slab(pageframe_t* pgfm, int obj_size, int offset)
{
	ASSERT(pgfm != NULL			\
	       && pgfm->pgtype == SLAB_ALLOCATOR	\
	       && (pgfm->meta).buddy_flags == PG_BUDDY_FLAG_HEAD);
	slab_t* ret;
	lock(&(pgfm->lk));
	uintptr_t va = pg2va(pgfm);
	int order = (pgfm->meta).buddy_order;
	ret = init_slab_xip((void*)va, (1<<order)*PAGE_SIZE, obj_size, offset);
	unlock(&(pgfm->lk));
	return ret;
}


kmem_cache_t* kmem_cache_alloc()
{
	return kmem_obj_alloc(&slab_kmm_cache);
}

void kmem_cache_free(kmem_cache_t* kmm_cache)
{
	kmem_obj_free(&slab_kmm_cache, kmm_cache);
	return;
}

int kmem_cache_init(kmem_cache_t* kmm_cache, int obj_size, int pg_order, int pgor_limit, int init_pgor_alloc)
{
	if(kmm_cache == NULL || obj_size <=0 				\
			|| !(pg_order >=0 && pg_order < MAX_ORDER) 	\
			|| pgor_limit <= 0 || init_pgor_alloc < 0	\
			|| init_pgor_alloc > pgor_limit			\
			|| obj_size >= ((1<<pg_order)*PAGE_SIZE)	)
		return -EFAULT;
	init_lock(&(kmm_cache->lk));
	INIT_LIST_HEAD(&(kmm_cache->free));
	INIT_LIST_HEAD(&(kmm_cache->full));
	INIT_LIST_HEAD(&(kmm_cache->partial));
	kmm_cache->obj_size = obj_size;
	kmm_cache->pg_order = pg_order;
	kmm_cache->pg_orlimit = pgor_limit;
	kmm_cache->pg_orhas = 0;
	kmm_cache->obj_num = 0;
	kmm_cache->color_offset = (int)(((uintptr_t)(kmm_cache)) & 0x7fffffff);
	for(int i=0;i<init_pgor_alloc;i++)
	{
		struct pageframe* pgfm = bkp_alloc(pg_order);
		if(pgfm == NULL)
			goto free_kmm_cache;
		set_pgfm_aslab(pgfm);
		slab_t* ret = init_slab(pgfm, obj_size, kmm_cache->color_offset);
		if(ret == NULL)
		{
			set_pgfm_asbuddy(pgfm);
			bkp_free(pgfm);
			goto free_kmm_cache;
		}
		ASSERT(ret->obj_all == ret->obj_free);
		kmm_cache->color_offset = (int)(((uintptr_t)(kmm_cache->color_offset) + 1) & 0x7fffffff);
		kmm_cache->obj_num += (ret->obj_free);
		kmm_cache->pg_orhas += 1;
		lock(&(pgfm->lk));
		list_add_tail(&((pgfm->meta).buddy_node),&(kmm_cache->free));
		unlock(&(pgfm->lk));
	}
	return 0;
free_kmm_cache:
	struct list_head* iterator;
	struct list_head* safe;
	list_for_each_safe(iterator, safe, &(kmm_cache->free))
	{
		pageframe_t* pgfm = list_entry(iterator, pageframe_t, meta.buddy_node);
		lock(&(pgfm->lk));
		list_del(&((pgfm->meta).buddy_node));
		pgfm->pgtype = PAGE_BUDDYALLOCATOR;
		ASSERT((pgfm->meta).buddy_flags == PG_BUDDY_FLAG_HEAD);
		unlock(&(pgfm->lk));
		bkp_free(pgfm);
	}
	return -ENOMEM;
}

void kmem_cache_deinit(kmem_cache_t* kmm_cache)
{
	if(kmm_cache == NULL)
		return;
	lock(&(kmm_cache->lk));
	struct list_head* iterator;
        struct list_head* safe;
	ASSERT(list_empty(&(kmm_cache->full)) == 1\
			&& list_empty(&(kmm_cache->partial)) == 1);
	list_for_each_safe(iterator, safe, &(kmm_cache->free))
	{
		pageframe_t* pgfm = list_entry(iterator, pageframe_t, meta.buddy_node);
                lock(&(pgfm->lk));
                list_del(&((pgfm->meta).buddy_node));
                pgfm->pgtype = PAGE_BUDDYALLOCATOR;
                ASSERT((pgfm->meta).buddy_flags == PG_BUDDY_FLAG_HEAD);
                unlock(&(pgfm->lk));
                bkp_free(pgfm);
	}
	unlock(&(kmm_cache->lk));
	memset(kmm_cache,0,sizeof(kmm_cache));
	return;
}

void* kmem_obj_alloc(kmem_cache_t* kmm_cache)
{
	if(kmm_cache == NULL)
		return NULL;
	lock(&(kmm_cache->lk));
	if(kmm_cache->obj_num == 0)
	{
		if(kmm_cache->pg_orhas >= kmm_cache->pg_orlimit)
		{
			unlock(&(kmm_cache->lk));
			return NULL;
		}
		struct pageframe* pgfm = bkp_alloc(kmm_cache->pg_order);
		if(pgfm == NULL)
		{
			unlock(&(kmm_cache->lk));
			return NULL;
		}
		set_pgfm_aslab(pgfm);
                slab_t* ret = init_slab(pgfm, kmm_cache->obj_size, kmm_cache->color_offset);
                if(ret == NULL)
                {
                        set_pgfm_asbuddy(pgfm);
                        bkp_free(pgfm);
			unlock(&(kmm_cache->lk));
			return NULL;
                }
		ASSERT(ret->obj_all == ret->obj_free);
                kmm_cache->color_offset = (int)(((uintptr_t)(kmm_cache->color_offset) + 1) & 0x7fffffff);
		kmm_cache->obj_num  += ret->obj_free;
                kmm_cache->pg_orhas += 1;
		lock(&(pgfm->lk));
                list_add_tail(&((pgfm->meta).buddy_node),&(kmm_cache->free));
                unlock(&(pgfm->lk));
	}
	if(!list_empty(&(kmm_cache->partial)))
	{
		pageframe_t* pgfm = list_first_entry(&(kmm_cache->partial), pageframe_t, meta.buddy_node);
		ASSERT(pgfm != NULL);
		lock(&(pgfm->lk));
		slab_t* slab_handler = (slab_t*)pg2va(pgfm);
		void* ret = slab_alloc(slab_handler);
		ASSERT(ret != NULL);
		if(slab_handler->obj_free == 0)
		{
			list_del(&((pgfm->meta).buddy_node));
			list_add_tail(&((pgfm->meta).buddy_node), &(kmm_cache->full));
		}
		unlock(&(pgfm->lk));
		kmm_cache->obj_num--;
		unlock(&(kmm_cache->lk));
		return ret;
	}
	pageframe_t* pgfm = list_first_entry(&(kmm_cache->free), pageframe_t, meta.buddy_node);
	ASSERT(pgfm != NULL);
	lock(&(pgfm->lk));
	slab_t* slab_handler = (slab_t*)pg2va(pgfm);
	void* ret = slab_alloc(slab_handler);
	ASSERT(ret != NULL);
	list_del(&((pgfm->meta).buddy_node));
	if(slab_handler->obj_free == 0)
		list_add_tail(&((pgfm->meta).buddy_node), &(kmm_cache->full));
	else
		list_add_tail(&((pgfm->meta).buddy_node), &(kmm_cache->partial));
	unlock(&(pgfm->lk));
	kmm_cache->obj_num--;
	unlock(&(kmm_cache->lk));
	return ret;
}

void kmem_obj_free(kmem_cache_t* kmm_cache, void* address)
{
	if(kmm_cache == NULL || address == NULL)
                return;
        lock(&(kmm_cache->lk));
	int size = (1 << kmm_cache->pg_order)*PAGE_SIZE;
	struct list_head* iterator;
	struct list_head* safe;
	list_for_each_safe(iterator, safe, &(kmm_cache->partial))
	{
		pageframe_t* pgfm = list_entry(iterator, pageframe_t, meta.buddy_node);
		lock(&(pgfm->lk));
		ASSERT(pgfm->pgtype == SLAB_ALLOCATOR\
				&& (pgfm->meta).buddy_flags == PG_BUDDY_FLAG_HEAD);
		slab_t* slab_handler = (slab_t*)(pg2va(pgfm));
		ASSERT(slab_handler->obj_all > slab_handler->obj_free);
		if((uintptr_t)address > (uintptr_t)slab_handler && (uintptr_t)address < ((uintptr_t)slab_handler) + size)
		{
			slab_free(slab_handler, address);
			if(slab_handler->obj_free == slab_handler->obj_all)
			{
				list_del(&((pgfm->meta).buddy_node));
				list_add_tail(&((pgfm->meta).buddy_node), &(kmm_cache->free));
			}
			unlock(&(pgfm->lk));
			kmm_cache->obj_num++;
			unlock(&(kmm_cache->lk));
			return;
		}
		unlock(&(pgfm->lk));
	}

	list_for_each_safe(iterator, safe, &(kmm_cache->full))
	{
		pageframe_t* pgfm = list_entry(iterator, pageframe_t, meta.buddy_node);
		lock(&(pgfm->lk));
		ASSERT(pgfm->pgtype == SLAB_ALLOCATOR\
                                && (pgfm->meta).buddy_flags == PG_BUDDY_FLAG_HEAD);
		slab_t* slab_handler = (slab_t*)(pg2va(pgfm));
		ASSERT(slab_handler->obj_free == 0);
		if((uintptr_t)address > (uintptr_t)slab_handler && (uintptr_t)address < ((uintptr_t)slab_handler) + size)
		{
			slab_free(slab_handler, address);
			list_del(&((pgfm->meta).buddy_node));
			if(slab_handler->obj_free == slab_handler->obj_all)
				list_add_tail(&((pgfm->meta).buddy_node), &(kmm_cache->free));
			else
				list_add_tail(&((pgfm->meta).buddy_node), &(kmm_cache->partial));
			unlock(&(pgfm->lk));
			kmm_cache->obj_num++;
			unlock(&(kmm_cache->lk));
			return;
		}
		unlock(&(pgfm->lk));
	}
	unlock(&(kmm_cache->lk));
	return;
}

void kmem_cached_info(kmem_cache_t* kmm_cache, kmm_cinfo* info)
{
	if(kmm_cache == NULL || info == NULL)
		return;
	lock(&(kmm_cache->lk));
	info->obj_num = kmm_cache->obj_num;
	info->obj_size = kmm_cache->obj_size;
	info->pg_orlimit = kmm_cache->pg_orlimit;
	info->pg_orhas = kmm_cache->pg_orhas;
	info->pg_order = kmm_cache->pg_order;
	unlock(&(kmm_cache->lk));
	return;
}
