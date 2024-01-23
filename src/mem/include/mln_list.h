
/*
 * Copyright (C) Niklaus F.Schen.
 */

#ifndef __MLN_LIST_H
#define __MLN_LIST_H

typedef struct mln_list_s {
    struct mln_list_s *next;
    struct mln_list_s *prev;
} mln_list_t;

/*
 * container_of and offsetof
 */
#define mln_offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define mln_container_of(ptr, type, member) ({\
    type *__rptr = NULL;\
    if ((ptr) != NULL) {\
        __rptr = (type *)((char *)((const typeof(((type *)0)->member) *)(ptr)) - mln_offsetof(type, member));\
    }\
    __rptr;\
})

#define mln_list_head(sentinel) ((sentinel)->prev)
#define mln_list_tail(sentinel) ((sentinel)->next)
#define mln_list_next(node)     ((node)->next)
#define mln_list_prev(node)     ((node)->prev)
#define mln_list_null()         {NULL, NULL}

extern void mln_list_add(mln_list_t *sentinel, mln_list_t *node);
extern void mln_list_remove(mln_list_t *sentinel, mln_list_t *node);
extern void mln_list_node_add_aft(mln_list_t* list,mln_list_t* node_1,mln_list_t* node_2);
extern void mln_list_node_add_bef(mln_list_t* list,mln_list_t* node_1,mln_list_t* node_2);
extern void mln_list_node_del(mln_list_t* list,mln_list_t* node);
extern void mln_list_ls_merge(mln_list_t* list_1,mln_list_t* list_2);


#endif


