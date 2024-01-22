#include "Los_type.h"
#include "platform.h"
#include "riscv.h"
//上面是依赖 [依赖哪些公共库和哪些模块]
//下面是实现 [实现了哪些公共库或者哪些模块]
#include "spinlock.h"


void init_lock(spinlock_t* lk,char *name)
{
    lk->value =0;
    lk->lock_name  =name;
    lk->int_save = -1;
}

void lock(spinlock_t* lk)
{
    uint64 x = 0;
    x = hwi_lock();
    while(__sync_lock_test_and_set(&(lk->value),1)) ;
    __sync_synchronize();
    lk->int_save = x;
}

void unlock(spinlock_t* lk)
{
    __sync_lock_release(&(lk->value));
    __sync_synchronize();
    hwi_restore(lk->int_save);
}