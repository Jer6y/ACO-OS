#ifndef __M_BLK_H
#define __M_BLK_H


typedef struct m_blk_s {
    m_type_t obj_type;  //magic number that will be checked for the strct
    int order;
    int free_pages;
    M_LOCK_T blk_lk;
    mln_list_t _list; //list for free pages
} m_blk_t;

#define m_blk_init_static(order) {OBJ_M_BLK,order,0,{NULL,NULL}}

#define m_blk_empty(blk_handler) ((blk_handler)->free_pages ==0)

M_API void  m_blk_init(m_blk_t* blk_handler,int order);

M_API void* m_blk_get_page(m_blk_t* blk_handler);

M_API void  m_blk_free_page(m_blk_t* blk_handler, void* addr_free);

M_API void* m_blk_free_and_check_merge(m_blk_t* blk_handler,void* addr_free);

M_API void  m_blk_free_page_disorder(m_blk_t* blk_handler,void* addr_free);

M_API void  m_blk_dump(m_blk_t* blk_handler);


#endif
