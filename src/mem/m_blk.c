#include "include/memory.h"

//not test yet
M_API void  m_blk_init(m_blk_t* blk_handler,int order)
{
#if (M_ARG_CHECK_FUNC == M_ENABLE)
    if(blk_handler == (m_blk_t*)0)
    {
        M_DEBUG(M_LEVEL_ERR,"blk_handler arg self error\n");
        M_EXIT(-1);
    }
    if(!(order>0 && order<= M_MAX_ORDER))
    {
        M_DEBUG(M_LEVEL_ERR,"blk_handler arg order error\n");
        M_EXIT(-1);
    }
#endif
    blk_handler->obj_type = OBJ_M_BLK;
    blk_handler->free_pages = 0;
    blk_handler->order = order;
    blk_handler->_list.next = (void*)0;
    blk_handler->_list.prev = (void*)0;
    M_LOCK_INIT(&(blk_handler->blk_lk),"mem_blk_pool lock");
}

M_API void* m_blk_get_page(m_blk_t* blk_handler)
{
#if (M_ARG_CHECK_FUNC == M_ENABLE)
    if(blk_handler == (m_blk_t*)0 || blk_handler->obj_type != OBJ_M_BLK)
    {
        M_DEBUG(M_LEVEL_ERR,"blk_handler arg self error\n");
        M_EXIT(-1);
    }
    if(!(blk_handler->order >0 && blk_handler->order <= M_MAX_ORDER))
    {
        M_DEBUG(M_LEVEL_ERR,"blk_handler arg order error\n");
        M_EXIT(-1);
    }
    if(blk_handler->free_pages <0 )
    {
        M_DEBUG(M_LEVEL_ERR,"blk_handler arg free_pages error\n");
        M_EXIT(-1);
    }
#endif
    M_LOCK(&(blk_handler->blk_lk));
    if(blk_handler->free_pages == 0)
    {
        M_UNLOCK(&(blk_handler->blk_lk));
        return (void*)0;
    }
    mln_list_t* node = mln_list_head(&(blk_handler->_list));
    mln_list_node_del(&(blk_handler->_list),node);
    blk_handler->free_pages--;
    M_UNLOCK(&(blk_handler->blk_lk));
    return (void*)node;
}

//find the node whose value_addr > addr_free for the first time
M_STATIC mln_list_t* m_blk_find(m_blk_t* blk_handler,M_ADDR_T addr_free)
{
    //assert(blk_handler != (m_blk_t*)0 && blk_handler->obj_type == OBJ_M_BLK);
    //assert(blk_handler->order >0 && blk_handler->order <= M_MAX_ORDER);
    //assert(blk_handler->free_pages < 0);
    mln_list_t* p_node = mln_list_head(&(blk_handler->_list));
    while(p_node != NULL)
    {
        M_ADDR_T addr_node = (M_ADDR_T)p_node;
        if(addr_node > addr_free)
        {
            return p_node;
        }
        p_node = mln_list_next(p_node);
    }
    return (mln_list_t *)0;
}

M_API void  m_blk_free_page(m_blk_t* blk_handler, void* addr_free)
{
#if (M_ARG_CHECK_FUNC == M_ENABLE)
    if(blk_handler == (m_blk_t*)0 || blk_handler->obj_type != OBJ_M_BLK)
    {
        M_DEBUG(M_LEVEL_ERR,"blk_handler arg self error\n");
        M_EXIT(-1);
    }
    if(!(blk_handler->order >0 && blk_handler->order <= M_MAX_ORDER))
    {
        M_DEBUG(M_LEVEL_ERR,"blk_handler arg order error\n");
        M_EXIT(-1);
    }
    if(blk_handler->free_pages <0 )
    {
        M_DEBUG(M_LEVEL_ERR,"blk_handler arg free_pages error\n");
        M_EXIT(-1);
    }
#endif
    M_ADDR_T addr_want_insert = (M_ADDR_T)addr_free;
    M_LOCK(&(blk_handler->blk_lk));
    blk_handler->free_pages++;
    mln_list_t* node = addr_free;
    node->next = (void*)0;
    node->prev = (void*)0;
    mln_list_t* node_tmp = m_blk_find(blk_handler,addr_want_insert);
    if(node_tmp == (mln_list_t*)0)
    {
        mln_list_add(&(blk_handler->_list),node);
    }
    else
    {
        mln_list_node_add_bef(&(blk_handler->_list),node_tmp,node);
    }
    M_UNLOCK(&(blk_handler->blk_lk));
}

M_API void* m_blk_free_and_check_merge(m_blk_t* blk_handler,void* addr_free)
{
#if (M_ARG_CHECK_FUNC == M_ENABLE)
    if(blk_handler == (m_blk_t*)0 || blk_handler->obj_type != OBJ_M_BLK)
    {
        M_DEBUG(M_LEVEL_ERR,"blk_handler arg self error\n");
        M_EXIT(-1);
    }
    if(!(blk_handler->order >0 && blk_handler->order <= M_MAX_ORDER))
    {
        M_DEBUG(M_LEVEL_ERR,"blk_handler arg order error\n");
        M_EXIT(-1);
    }
    if(blk_handler->free_pages <0 )
    {
        M_DEBUG(M_LEVEL_ERR,"blk_handler arg free_pages error\n");
        M_EXIT(-1);
    }
#endif
    M_ADDR_T addr_want_insert = (M_ADDR_T)addr_free;
    M_LOCK(&(blk_handler->blk_lk));
    mln_list_t* node = addr_free;
    node->next = (void*)0;
    node->prev = (void*)0;
    mln_list_t* bef_node = (mln_list_t*)0;
    mln_list_t* aft_node = (mln_list_t*)0;
    mln_list_t* node_tmp = m_blk_find(blk_handler,addr_want_insert);
    if(node_tmp == NULL)
    {
        bef_node = mln_list_tail(&(blk_handler->_list));
        M_ADDR_T bef_node_addr = (M_ADDR_T)bef_node;
        if(bef_node_addr + (1<<(blk_handler->order-1))*M_PAGE_SIZE == addr_want_insert)
        {
            mln_list_node_del(&(blk_handler->_list),bef_node);
            blk_handler->free_pages--;
            M_UNLOCK(&(blk_handler->blk_lk));
            return (void*)bef_node_addr;
        }
        else
        {
            mln_list_add(&(blk_handler->_list),node);
            blk_handler->free_pages++;
            M_UNLOCK(&(blk_handler->blk_lk));
            return (void*)0;
        }
    }
    else
    {
        aft_node = node_tmp;
        bef_node = mln_list_prev(node_tmp);
        if(bef_node != NULL)
        {
            M_ADDR_T bef_node_addr = (M_ADDR_T)bef_node;
            if(bef_node_addr + (1<<(blk_handler->order-1))*M_PAGE_SIZE == addr_want_insert)
            {
                mln_list_node_del(&(blk_handler->_list),bef_node);
                blk_handler->free_pages--;
                M_UNLOCK(&(blk_handler->blk_lk));
                return (void*)bef_node_addr;
            }
        }
        M_ADDR_T aft_node_addr = (M_ADDR_T)aft_node;
        if(addr_want_insert + (1<<(blk_handler->order-1))*M_PAGE_SIZE == aft_node_addr)
        {
            mln_list_node_del(&(blk_handler->_list),aft_node);
            blk_handler->free_pages--;
            M_UNLOCK(&(blk_handler->blk_lk));
            return (void*)addr_want_insert;
        }

        mln_list_node_add_bef(&(blk_handler->_list),aft_node,node);
        blk_handler->free_pages++;
        M_UNLOCK(&(blk_handler->blk_lk));
        return (void*)0;
    }
}

M_API void  m_blk_free_page_disorder(m_blk_t* blk_handler,void* addr_free)
{
#if (M_ARG_CHECK_FUNC == M_ENABLE)
    if(blk_handler == (m_blk_t*)0 || blk_handler->obj_type != OBJ_M_BLK)
    {
        M_DEBUG(M_LEVEL_ERR,"blk_handler arg self error\n");
        M_EXIT(-1);
    }
    if(!(blk_handler->order >0 && blk_handler->order <= M_MAX_ORDER))
    {
        M_DEBUG(M_LEVEL_ERR,"blk_handler arg order error\n");
        M_EXIT(-1);
    }
    if(blk_handler->free_pages <0 )
    {
        M_DEBUG(M_LEVEL_ERR,"blk_handler arg free_pages error\n");
        M_EXIT(-1);
    }
#endif
    M_ADDR_T addr_want_insert = (M_ADDR_T)addr_free;
    M_LOCK(&(blk_handler->blk_lk));
    blk_handler->free_pages++;
    mln_list_t* node = addr_free;
    node->next = (void*)0;
    node->prev = (void*)0;
    mln_list_add(&(blk_handler->_list),node);
    M_UNLOCK(&(blk_handler->blk_lk));
    return;
}

M_STATIC void m_blk_dump_list(m_blk_t* blk_handler)
{
    //assert(blk_handler != (m_blk_t*)0 && blk_handler->obj_type == OBJ_M_BLK);
    mln_list_t* p_node = mln_list_head(&(blk_handler->_list));
    while(p_node!= (mln_list_t*)0)
    {
        printk("free_addrs: %p\n",p_node);
        p_node = mln_list_next(p_node);
    }
}

M_API void  m_blk_dump(m_blk_t* blk_handler)
{
#if (M_ARG_CHECK_FUNC == M_ENABLE)
    if(blk_handler == (m_blk_t*)0 || blk_handler->obj_type != OBJ_M_BLK)
    {
        M_DEBUG(M_LEVEL_ERR,"dump m_blk error order : %d\n",blk_handler->order);
        M_EXIT(-1);
    }
#endif
    M_LOCK(&(blk_handler->blk_lk));
    printk("===========debug_blk_handler===================\n");
    printk("order : %d\n",blk_handler->order);
    printk("free_pages : %d\n",blk_handler->free_pages);
    printk("free_size : 0x%x\n",blk_handler->free_pages *M_PAGE_SIZE * (1<<(blk_handler->order-1)));
    m_blk_dump_list(blk_handler);
    M_UNLOCK(&(blk_handler->blk_lk));
}