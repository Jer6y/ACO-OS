#include "include/memory.h"

M_API void  m_pool_init(m_pool_t* pool_handler,void* start_addr,int length)
{
#if (M_ARG_CHECK_FUNC == M_ENABLE)
    if(pool_handler == (m_pool_t*)0)
    {
        M_DEBUG(M_LEVEL_ERR,"pool_handler arg self error\n");
        M_EXIT(-1);
    }
    if(start_addr == (void*)0)
    {
        M_DEBUG(M_LEVEL_ERR,"pool_handler arg addr error\n");
        M_EXIT(-1);
    }
#endif
    if(length % M_PAGE_SIZE != 0)
    {
        M_DEBUG(M_LEVEL_WARN,"pool_handler arg length not page-aligned\n");
        length = length - (length%M_PAGE_SIZE);
    }
    pool_handler->obj_type = OBJ_M_POOL;
    pool_handler->free_size = length;
    M_LOCK_INIT(&(pool_handler->pool_lock),"buddy system alloc");
    pool_handler->_node.prev = NULL;
    pool_handler->_node.next = NULL;
    for(int i=0;i<M_MAX_ORDER;i++)
    {
        m_blk_init(&(pool_handler->blk_orders[i]),i+1);
    }
    M_ADDR_T addr = (M_ADDR_T)start_addr;
    while(length != 0)
    {
        for(int i= M_MAX_ORDER;i>0;i--)
        {
            if(length >= (1<<(i-1))*M_PAGE_SIZE)
            {
                m_blk_free_page(&(pool_handler->blk_orders[i-1]),(void*)start_addr);
                length -= ((1<<(i-1))*M_PAGE_SIZE);
                start_addr += ((1<<(i-1))*M_PAGE_SIZE);
                break;
            }
        }
    }
}
M_API void* m_pool_alloc(m_pool_t* pool_handler,int order)
{
#if (M_ARG_CHECK_FUNC == M_ENABLE)
    if(pool_handler == (m_pool_t*)0 || pool_handler->obj_type != OBJ_M_POOL)
    {
        M_DEBUG(M_LEVEL_ERR,"pool_handler arg self error\n");
        M_EXIT(-1);
    }
    if(pool_handler->free_size <0)
    {
        M_DEBUG(M_LEVEL_ERR,"pool_handler arg free_size error\n");
        M_EXIT(-1);
    }
#endif
    if(!(order>0 && order <= M_MAX_ORDER))
    {
        M_DEBUG(M_LEVEL_WARN,"m_pool_alloc arg order error\n");
        return (void*)0;
    }
    M_LOCK(&(pool_handler->pool_lock));
    if(pool_handler->free_size ==0)
    {
        M_UNLOCK(&(pool_handler->pool_lock));
        return (void*)0;
    }
    int r_order = order;
    void* addr_aloc = (void*)0;
    while(r_order <= M_MAX_ORDER)
    {
        addr_aloc = m_blk_get_page(&(pool_handler->blk_orders[r_order-1]));
        if(addr_aloc != (void*)0) 
        {
            break;
        }
        r_order++;
    }
    if(r_order > M_MAX_ORDER)
    {
        M_UNLOCK(&(pool_handler->pool_lock));
        return (void*)0;   
    }
    int all_page_num = (1<<(r_order-1));
    int want_page_num = (1<<(order-1));
    int rest_page_num = all_page_num - want_page_num;
    M_ADDR_T ret = (M_ADDR_T)addr_aloc;
    M_ADDR_T ret_cpy = ret; 
    ret_cpy += (1<<(order-1)) * M_PAGE_SIZE;
    for(int i = 1;i<=M_MAX_ORDER;i++)
    {
        if(rest_page_num & (1<<(i-1)))
        {
            rest_page_num -= (1<<(i-1));
            m_blk_free_page(&(pool_handler->blk_orders[i-1]),(void*)ret_cpy); 
            ret_cpy += (1<<(i-1))*M_PAGE_SIZE;
        }
    }
    if(ret + (1<<(r_order-1))*M_PAGE_SIZE != ret_cpy)
    {
        M_DEBUG(M_LEVEL_ERR,"err occur for pool_alloc \n");
        M_EXIT(-1);
    }
    pool_handler->free_size -= ((1<<(order-1))*M_PAGE_SIZE);
    M_UNLOCK(&(pool_handler->pool_lock));
    return (void*)ret;
}

M_API void  m_pool_free(m_pool_t* pool_handler,void* free_addr,int order)
{
#if (M_ARG_CHECK_FUNC == M_ENABLE)
    if(pool_handler == (m_pool_t*)0 || pool_handler->obj_type != OBJ_M_POOL)
    {
        M_DEBUG(M_LEVEL_ERR,"pool_handler arg self error\n");
        M_EXIT(-1);
    }
    if(pool_handler->free_size <0)
    {
        M_DEBUG(M_LEVEL_ERR,"pool_handler arg free_size error\n");
        M_EXIT(-1);
    }
    if(free_addr == (void*)0)
    {
        M_DEBUG(M_LEVEL_ERR,"poll_free arg free_addr error\n");
        M_EXIT(-1);
    }
    if(!(order>0 && order <= M_MAX_ORDER))
    {
        M_DEBUG(M_LEVEL_ERR,"m_pool_alloc arg order error\n");
        M_EXIT(-1);
    }
#endif
    M_LOCK(&(pool_handler->pool_lock));
    int r_order = order;
    void* r_free_addr = free_addr;
    if(r_order == M_MAX_ORDER)
    {
        pool_handler->free_size += (1<<(order-1))*M_PAGE_SIZE;
        m_blk_free_page_disorder(&(pool_handler->blk_orders[r_order-1]),r_free_addr);
        M_UNLOCK(&(pool_handler->pool_lock));
        return;
    }
    while(r_order < M_MAX_ORDER)
    {
        r_free_addr = m_blk_free_and_check_merge(&(pool_handler->blk_orders[r_order-1]),r_free_addr);
        if(r_free_addr == (void*)0) 
        {
            pool_handler->free_size += (1<<(order-1))*M_PAGE_SIZE;
            M_UNLOCK(&(pool_handler->pool_lock));
            return;
        }
        r_order++;
    }
    m_blk_free_page_disorder(&(pool_handler->blk_orders[r_order-1]),r_free_addr);
    pool_handler->free_size += (1<<(order-1))*M_PAGE_SIZE;
    M_UNLOCK(&(pool_handler->pool_lock));
}

M_API void  m_pool_dump(m_pool_t* pool_handler)
{
#if (M_ARG_CHECK_FUNC == M_ENABLE)
    if(pool_handler == (m_pool_t*)0 || pool_handler->obj_type != OBJ_M_POOL)
    {
        M_DEBUG(M_LEVEL_ERR,"m_pool_dump error in %p\n",pool_handler);
        M_EXIT(-1);
    }
#endif
    printf("========================dump_pool=====================\n");
    printf("total free_size: 0x%x\n",pool_handler->free_size);
    for(int i=0;i<M_MAX_ORDER;i++)
    {
        m_blk_dump(&(pool_handler->blk_orders[i]));
    }
    printf("=======================dump_pool_end=================\n");

}