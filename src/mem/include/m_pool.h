#ifndef __M_POOL_H
#define __M_POOL_H

typedef struct m_pool_s {
    m_type_t obj_type;
    m_blk_t  blk_orders[M_MAX_ORDER];
    int      free_size;
    M_LOCK_T pool_lock;
    mln_list_t _node;
} m_pool_t;

M_API void  m_pool_init(m_pool_t* pool_handler,void* start_addr,int length);

M_API void* m_pool_alloc(m_pool_t* pool_handler,int order);

M_API void  m_pool_free(m_pool_t* pool_handler,void* free_addr,int order);

M_API void  m_pool_dump(m_pool_t* pool_handler);


#endif