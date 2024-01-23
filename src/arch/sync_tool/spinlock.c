#include "Los_type.h"
#include "Los_err.h"
#include "platform.h"
#include "stdarg.h"
#include "string.h"
#include "uart.h"
#include "riscv.h"
#include "assert.h"
//上面是依赖 [依赖哪些公共库和哪些模块]
//下面是实现 [实现了哪些公共库或者哪些模块]
#include "spinlock.h"


void init_lock(spinlock_t* lk,char *name)
{
    ASSERT(lk != NULL && name != NULL);
    lk->value =0;
    lk->lock_name  =name;
    lk->int_save = -1;
}

void lock(spinlock_t* lk)
{
    ASSERT(lk != NULL);
    uint64 x = 0;
    x = hwi_lock();
    while(__sync_lock_test_and_set(&(lk->value),1)) ;
    __sync_synchronize();
    lk->int_save = x;
}

void unlock(spinlock_t* lk)
{
    ASSERT(lk != NULL);
    ASSERT(lk->int_save != -1);
    int int_save = lk->int_save;
    lk->int_save = -1;
    __sync_lock_release(&(lk->value));
    __sync_synchronize();
    hwi_restore(int_save);
}