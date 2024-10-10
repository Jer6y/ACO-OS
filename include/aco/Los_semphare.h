/*
 *
 *  semphare相关的API
 */

#ifndef LOS_SEMPHARE_H
#define LOS_SEMPHARE_H

typedef struct semphare_s {
    los_type_t obj_type;
    spinlock_t lk;
    int        val;
    mln_list_t _list; //which want but sleeping on
} semphare_t;


LOS_ERR_T sem_post(semphare_t* sem);

LOS_ERR_T sem_wait(semphare_t* sem);

LOS_ERR_T sem_init(semphare_t* sem, int val);



#endif