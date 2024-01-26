/*
 *
 *  mutex 锁相关的API
 */
#ifndef LOS_MUTEX_H
#define LOS_MUTEX_H

//mutex属性 - 没拿到前就block
#define MUTEX_ATTR_BLOCK    (1<<0u)

//mutex属性 - 没拿到临界区即使返回
#define MUTEX_ATTR_NONBLK   (1<<1u)

typedef uint8 mx_attr_t;

typedef struct mutex_s {
    los_type_t obj_type;
    spinlock_t lk;
    mx_attr_t  attr;
    int        val;
    mln_list_t _list;  //which want but sleeping on
} mutex_t;

LOS_ERR_T mutex_lock(mutex_t* mutex);

LOS_ERR_T mutex_unlock(mutex_t* mutex);

LOS_ERR_T mutex_init(mutex_t* mutex, mx_attr_t attr);

#endif