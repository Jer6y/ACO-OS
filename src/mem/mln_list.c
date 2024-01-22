
/*
 * Copyright (C) Niklaus F.Schen.
 */

#include "include/memory.h"

#define MLN_CHAIN_FUNC_DECLARE(prefix,type,ret_attr,func_attr); \
    ret_attr prefix##_chain_add(type **head, type **tail, type *node) func_attr;\
    ret_attr prefix##_chain_del(type **head, type **tail, type *node) func_attr;

#define MLN_CHAIN_FUNC_DEFINE(prefix,type,ret_attr,prev_ptr,next_ptr); \
    ret_attr prefix##_chain_add(type **head, type **tail, type *node) \
    {\
        if (head == NULL || tail == NULL || node == NULL) return;\
        node->prev_ptr = node->next_ptr = NULL;\
        if (*head == NULL) {\
            *head = *tail = node;\
            return ;\
        }\
        (*tail)->next_ptr = node;\
        node->prev_ptr = (*tail);\
        *tail = node;\
    }\
    \
    ret_attr prefix##_chain_del(type **head, type **tail, type *node) \
    {\
        if (head == NULL || tail == NULL || node == NULL) return;\
        if (*head == node) {\
            if (*tail == node) {\
                *head = *tail = NULL;\
            } else {\
                *head = node->next_ptr;\
                (*head)->prev_ptr = NULL;\
            }\
        } else {\
            if (*tail == node) {\
                *tail = node->prev_ptr;\
                (*tail)->next_ptr = NULL;\
            } else {\
                node->prev_ptr->next_ptr = node->next_ptr;\
                node->next_ptr->prev_ptr = node->prev_ptr;\
            }\
        }\
        node->prev_ptr = node->next_ptr = NULL;\
    }

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


MLN_CHAIN_FUNC_DECLARE(mln_list, mln_list_t, static inline void,);
MLN_CHAIN_FUNC_DEFINE(mln_list, mln_list_t, static inline void, prev, next);

void mln_list_add(mln_list_t *sentinel, mln_list_t *node)
{
    mln_list_chain_add(&mln_list_head(sentinel), &mln_list_tail(sentinel), node);
}

void mln_list_remove(mln_list_t *sentinel, mln_list_t *node)
{
    mln_list_chain_del(&mln_list_head(sentinel), &mln_list_tail(sentinel), node);
}

void mln_list_node_add_aft(mln_list_t* list,mln_list_t* node_1,mln_list_t* node_2)
{
    if(node_1 == NULL || node_2 == NULL || list == NULL) return;

    if(node_1->next!=NULL)
        node_1->next->prev = node_2;
    node_2->next = node_1->next;

    node_1->next = node_2;
    node_2->prev = node_1;
    if(list->next == node_1)
        list->next = node_2;    
}

void mln_list_node_add_bef(mln_list_t* list,mln_list_t* node_1,mln_list_t* node_2)
{
    if(node_1== NULL || node_2 == NULL || list ==NULL) return;
    if(node_1->prev != NULL)
        node_1->prev->next = node_2;
    node_2->prev = node_1->prev;
    node_2->next = node_1;
    node_1->prev = node_2;
    if(list->prev == node_1)
        list->prev = node_2;
}

void mln_list_node_del(mln_list_t* list,mln_list_t* node)
{
    if(list == NULL || node == NULL) return;
    if(node == list->prev)
        list->prev = node->next;
    if(node == list->next)
        list->next = node->prev;
    if(node->prev !=NULL)
        node->prev->next = node->next;
    if(node->next != NULL)
        node->next->prev = node->prev;
    node->prev = NULL;
    node->next = NULL;
}

void mln_list_ls_merge(mln_list_t* list_1,mln_list_t* list_2)
{
    if(list_1 == NULL || list_2 == NULL) return;

    mln_list_t* l1_tail = mln_list_tail(list_1);
    mln_list_t* l2_head = mln_list_head(list_2);
    if(l1_tail == NULL)
    {
        list_1->next = list_2->next;
        list_1->prev = list_2->prev;
        list_2->next = NULL;
        list_2->prev = NULL;
    }
    else
    {
        if(l2_head == NULL) return;
        l1_tail->next = l2_head;
        l2_head->prev = l1_tail;
        //list_1->prev = list_1->prev;
        list_1->next = list_2->next; 
        list_2->next = NULL;
        list_2->prev = NULL;
    }
}


