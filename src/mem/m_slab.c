#include "include/memory.h"

M_API m_slab_t* m_slab_init(void* start,int order,int obj_size)
{
#if (M_ARG_CHECK_FUNC == M_ENABLE)
    if(!(order >0 && order <= M_MAX_ORDER))
    {
        M_DEBUG(M_LEVEL_ERR,"m_slab_init order error : %d\n",order);
        M_EXIT(-1);
    }
    if(start == (void*)0)
    {
        M_DEBUG(M_LEVEL_ERR,"m_slab_init start error : null\n",start);
        M_EXIT(-1);
    }
    if(!(obj_size >0 && obj_size <M_PAGE_SIZE))
    {
        M_DEBUG(M_LEVEL_ERR,"m_slab_init obj_size error: %d\n",obj_size);
        M_EXIT(-1);
    }
#endif
    int length = (1<<(order-1))*M_PAGE_SIZE;
    m_slab_t* slab_handler = (m_slab_t*)start;
    slab_handler->_node.next = (void*)0;
    slab_handler->_node.prev = (void*)0;
    slab_handler->r_start = start;
    slab_handler->order = order;
    slab_handler->obj_type = OBJ_M_SLAB;
    slab_handler->obj_size = obj_size;
    int obj_num =  (length - sizeof(m_slab_t))/(sizeof(int)+obj_size);
    int padding =  (length - sizeof(m_slab_t))%(sizeof(int)+obj_size);
    //may be we can do something here to adapt cacheline like a real slab_allocator
    //but now we do nothing
    if(obj_num < 10)
    {
        return (m_slab_t*)0;
    }
    slab_handler->obj_all = obj_num;
    slab_handler->obj_free = obj_num;
    slab_handler->mem_index_start = (int*)(((char*)start)+sizeof(m_slab_t));
    slab_handler->mem_obj_start = (void*)((slab_handler->mem_index_start)+obj_num);
    for(int i=0;i<obj_num;i++)
    {
        if(i != obj_num-1)
        {
            (slab_handler->mem_index_start)[i] = i+1;
        }
        else
        {
            (slab_handler->mem_index_start)[i] = i;
        }
    }
    slab_handler->mem_free_head = 0;
    slab_handler->mem_free_tail = obj_num-1;
    M_LOCK_INIT(&(slab_handler->slab_lock),"slab_lock");
    return slab_handler;
}

M_API void      m_slab_free(m_slab_t* slab_handler,m_pool_t* pool_handler)
{
#if (M_ARG_CHECK_FUNC == M_ENABLE)
    if(slab_handler == (m_slab_t*)0 || slab_handler->obj_type != OBJ_M_SLAB)
    {
        M_DEBUG(M_LEVEL_ERR,"m_slab_free: a null ptr %s %d\n",__FILE__,__LINE__);
        M_EXIT(-1);
    }
    if(pool_handler == (m_pool_t*)0 || pool_handler->obj_type != OBJ_M_POOL)
    {
	    M_DEBUG(M_LEVEL_ERR,"m_slab_free: a null ptr %s %d\n",__FILE__,__LINE__);
	    M_EXIT(-1);
    }
    if(!(slab_handler->order >0 && slab_handler->order <= M_MAX_ORDER))
    {
        M_DEBUG(M_LEVEL_ERR,"m_slab_free: a error order for slab_handler %s %d\n",__FILE__,__LINE__);
        M_EXIT(-1);
    }
#endif
    void* start = slab_handler->r_start;
    int   order = slab_handler->order;
    int   length = (1<<(order-1))*M_PAGE_SIZE;
    //clear the memory
    char* clear_ptr = (char*)start;
    for(int i=0;i<length;i++)
    {
        clear_ptr[i]=0;
    }
    m_pool_free(pool_handler,start,order);
}

M_API void*     m_slab_obj_alloc(m_slab_t* slab_handler)
{
#if (M_ARG_CHECK_FUNC == M_ENABLE)
    if(slab_handler == (m_slab_t*)0 || slab_handler->obj_type != OBJ_M_SLAB)
    {
        M_DEBUG(M_LEVEL_ERR,"m_slab_free: a null ptr %s %d\n",__FILE__,__LINE__);
        M_EXIT(-1);
    }
#endif
    M_LOCK(&(slab_handler->slab_lock));
    if(slab_handler->obj_free == 0)
    {
        M_UNLOCK(&(slab_handler->slab_lock));
        return (void*)0;
    }
    int index = slab_handler->mem_free_head;
    if(index == -1)
    {
        M_DEBUG(M_LEVEL_WARN,"m_slab_free: a illeagl slab_handler state %s %d\n",__FILE__,__LINE__);
        M_UNLOCK(&(slab_handler->slab_lock));
        M_EXIT(-1);
    }
    void* ret = (void*)((char*)(slab_handler->mem_obj_start) + (slab_handler->obj_size)*index);
    slab_handler->mem_free_head = slab_handler->mem_index_start[index];
    slab_handler->mem_index_start[index] = 0xffffffff;
    slab_handler->obj_free--;
    if(slab_handler->obj_free ==0)
    {
        slab_handler->mem_free_head = -1;
        slab_handler->mem_free_tail = -1;
    }
    M_UNLOCK(&(slab_handler->slab_lock));
    return ret;
}

M_API void m_slab_obj_free(m_slab_t* slab_handler,void* start_addr)
{
#if (M_ARG_CHECK_FUNC == M_ENABLE)
    if(slab_handler == (m_slab_t*)0 || slab_handler->obj_type != OBJ_M_SLAB)
    {
        M_DEBUG(M_LEVEL_ERR,"m_slab_free: a null ptr %s %d\n",__FILE__,__LINE__);
        M_EXIT(-1);
    }
    if(start_addr == (void*)0)
    {
        M_DEBUG(M_LEVEL_ERR,"m_slab_free: a null free ptr %s %d\n",__FILE__,__LINE__);
        M_EXIT(-1);
    }
#endif
    if((char*)start_addr < (char*)(slab_handler->mem_obj_start))
    {
        M_DEBUG(M_LEVEL_WARN,"m_slab_free: a illeagl free ptr %s %d\n",__FILE__,__LINE__);
        return;
    }
    int index = (int)((char*)start_addr - (char*)(slab_handler->mem_obj_start));
    if(index % slab_handler->obj_size != 0)
    {
        M_DEBUG(M_LEVEL_WARN,"m_slab_free: a illeagl free ptr %s %d\n",__FILE__,__LINE__);
        return;
    }
    index /= slab_handler->obj_size;
    if(index >= slab_handler->obj_all)
    {
        M_DEBUG(M_LEVEL_WARN,"m_slab_free: a illeagl free ptr %s %d\n",__FILE__,__LINE__);
        return;   
    }
    M_LOCK(&(slab_handler->slab_lock));
    if((slab_handler->mem_index_start)[index] != 0xffffffff)
    {
        M_DEBUG(M_LEVEL_WARN,"m_slab_free: a illeagl free ptr %s %d\n",__FILE__,__LINE__);
        M_UNLOCK(&(slab_handler->slab_lock));
        return;   
    }
    //clear the object 
    char* clear_ptr = (char*)start_addr;
    for(int i=0;i<slab_handler->obj_size;i++)
    {
        clear_ptr[i]=0;
    }

    if(slab_handler->obj_free ==0)
    {
        if(slab_handler->mem_free_head !=-1 || slab_handler->mem_free_tail != -1)
        {
            M_DEBUG(M_LEVEL_WARN,"m_slab_free: a illeagl slab_handler state %s %d\n",__FILE__,__LINE__);
            M_UNLOCK(&(slab_handler->slab_lock));
            M_EXIT(-1);
        }
        slab_handler->mem_free_head = index;
        slab_handler->mem_free_tail = index;
        (slab_handler->mem_index_start)[index] = index;
    }
    else
    {
        if(slab_handler->mem_free_head == -1 || slab_handler->mem_free_tail == -1)
        {
            M_DEBUG(M_LEVEL_WARN,"m_slab_free: a illeagl slab_handler state %s %d\n",__FILE__,__LINE__);
            M_UNLOCK(&(slab_handler->slab_lock));
            M_EXIT(-1);
        }
        (slab_handler->mem_index_start)[slab_handler->mem_free_tail] = index;
        (slab_handler->mem_index_start)[index] = index;
        slab_handler->mem_free_tail = index;
    }
    slab_handler->obj_free++;
    M_UNLOCK(&(slab_handler->slab_lock));
    return;
}

M_API m_slab_t* m_slab_init_align(void* start, int order, int obj_size, int align_size)
{
#if (M_ARG_CHECK_FUNC == M_ENABLE)
    if(!(order >0 && order <= M_MAX_ORDER))
    {
        M_DEBUG(M_LEVEL_ERR,"m_slab_init order error : %d\n",order);
        M_EXIT(-1);
    }
    if(start == (void*)0)
    {
        M_DEBUG(M_LEVEL_ERR,"m_slab_init start error : null\n",start);
        M_EXIT(-1);
    }
    if(!(obj_size >0 && obj_size <M_PAGE_SIZE))
    {
        M_DEBUG(M_LEVEL_ERR,"m_slab_init obj_size error: %d\n",obj_size);
        M_EXIT(-1);
    }

    if(!(align_size >0 && align_size <M_PAGE_SIZE) || align_size >= obj_size)
    {
        M_DEBUG(M_LEVEL_ERR,"m_slab_init align_size error: %d\n",align_size);
        M_EXIT(-1);
    }
    
#endif
    if(obj_size % align_size != 0)
    {
        obj_size += align_size - (obj_size % align_size);
    }
    int length = (1<<(order-1))*M_PAGE_SIZE;
    m_slab_t* slab_handler = (m_slab_t*)start;
    slab_handler->_node.next = (void*)0;
    slab_handler->_node.prev = (void*)0;
    slab_handler->r_start = start;
    slab_handler->order = order;
    slab_handler->obj_type = OBJ_M_SLAB;
    slab_handler->obj_size = obj_size;

    M_ADDR_T search_addr = (M_ADDR_T)start + sizeof(m_slab_t);
    while(search_addr % sizeof(int) != 0)
    {
        search_addr ++;
    }
    if(search_addr >= (M_ADDR_T)start + length)
    {
        return (m_slab_t *)0;
    }

    int obj_num =  ((M_ADDR_T)start + length - search_addr)/(sizeof(int)+obj_size);
    
    if(obj_num < 10)
    {
        return (m_slab_t*)0;
    }

    slab_handler->mem_index_start = (int*)search_addr;

    search_addr += sizeof(int)*obj_num;
    
    while(search_addr % align_size !=0) 
    {
        search_addr++;
    }

    if(search_addr >= (M_ADDR_T)start + length)
    {
        return (m_slab_t *)0;
    }

    int o_objnum = ((M_ADDR_T)start + length - search_addr)/obj_size;

    if(o_objnum < 10)
    {
        return (m_slab_t*)0;
    }
    int r_objnum = (o_objnum < obj_num)? o_objnum: obj_num;
    slab_handler->obj_all = r_objnum;
    slab_handler->obj_free = r_objnum;
    slab_handler->mem_obj_start = (void*)search_addr;
    for(int i=0;i<r_objnum;i++)
    {
        if(i == r_objnum -1)
        {
            (slab_handler->mem_index_start)[i] = i;
        }
        else
        {
            (slab_handler->mem_index_start)[i] = i+1;    
        }
    }
    slab_handler->mem_free_head = 0;
    slab_handler->mem_free_tail = r_objnum-1;
    M_LOCK_INIT(&(slab_handler->slab_lock),"slab_lock");
    return slab_handler;
}


M_API void  dump_slab(m_slab_t * slab_handler)
{
#if (M_ARG_CHECK_FUNC == M_ENABLE)
    if(slab_handler == (m_slab_t*)0 || slab_handler->obj_type != OBJ_M_SLAB)
    {
        M_DEBUG(M_LEVEL_ERR,"slab_handler err %p\n",slab_handler);
        M_EXIT(-1);
    }
#endif
    printf("===============dump slab start==================\n");
    M_LOCK(&(slab_handler->slab_lock));
    printf("slab obj_size: %d\n",slab_handler->obj_size);
    printf("slab obj_all: %d\n",slab_handler->obj_all);
    printf("slab obj_free: %d\n",slab_handler->obj_free);
    printf("slab adrs_start: 0x%p\n",slab_handler->r_start);
    printf("slab adrs_lenth: 0x%x\n",(1<<(slab_handler->order -1))*M_PAGE_SIZE);
    printf("slab index_start:0x%p\n",slab_handler->mem_index_start);
    printf("slab obj_start: 0x%p\n",slab_handler->mem_obj_start);
    int p = slab_handler->mem_free_head;
    printf("=====================================\n");
    while(1)
    {
        printf("free obj_site : %d\n",p);
        if((slab_handler->mem_index_start)[p] == p)
        {
            break;
        }
        p = (slab_handler->mem_index_start)[p];
    }
    printf("=====================================\n");
    for(int i= 0;i<slab_handler->obj_all;i++)
    {
        if(slab_handler->mem_index_start[i] ==0xffffffff)
        {
            printf("alloc obj_site: %d\n",i);
        }
    }
    printf("=====================================\n");
    M_UNLOCK(&(slab_handler->slab_lock));
}