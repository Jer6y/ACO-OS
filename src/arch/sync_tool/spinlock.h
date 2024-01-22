#ifndef SPINLOCK_H
#define SPINLOCK_H

typedef struct spinlock_s {
    int value;
    int int_save;
    //for debug
    char* lock_name;
} spinlock_t;

void init_lock(spinlock_t* lk,char *name);

void lock(spinlock_t* lk);

void unlock(spinlock_t* lk);

#endif