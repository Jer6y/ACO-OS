#include "Los_type.h"
#include "Los_err.h"
#include "platform.h"
#include "stdarg.h"
#include "string.h"
#include "uart.h"
#include "riscv.h"
#include "plic.h"
#include "clint.h"
#include "spinlock.h"
#include "assert.h"
#include "memory.h"
#include "tick.h"
#include "sched.h"
#include "Los_signal.h"
#include "proc.h"
#include "Los_timer.h"
#include "Los_mutex.h"

LOS_ERR_T mutex_lock(mutex_t* mutex)
{
    if(mutex == NULL)
    {
        return ERR_PTR_NULL;
    }
    if(mutex->obj_type != LOS_MUTEX)
    {
        return ERR_OBJ_MAGIC_ERR;
    }
    uint64 x = hwi_lock();
    lock(&(mutex->lk));
    if(mutex->val ==0)
    {
        mutex->val = 1;
        unlock(&(mutex->lk));
        hwi_restore(x);
        return ERR_NONE;
    }
    if(mutex->attr == MUTEX_ATTR_NONBLK)
    {
        unlock(&(mutex->lk));
        hwi_restore(x);
        return ERR_MUTEX_CRTICAL;
    }
    if(tsk_trap(ID_MUTEX, mutex) == 0x65791)
    {
        unlock(&(mutex->lk));
        hwi_restore(x);
        return ERR_SCHED_LOCKED;
    }
    
    //这里是有点trick的
    //跨越维度的lock
    RUNNING_PROC->on_mutex =0;
    unlock(&(mutex->lk));
    hwi_restore(x);
    return ERR_NONE;
}

LOS_ERR_T mutex_unlock(mutex_t* mutex)
{
    if(mutex == NULL)
    {
        return ERR_PTR_NULL;
    }
    if(mutex->obj_type != LOS_MUTEX)
    {
        return ERR_OBJ_MAGIC_ERR;
    }
    uint64 x = hwi_lock();
    lock(&(mutex->lk));
    if(mutex->val == 0)
    {
        unlock(&(mutex->lk));
        hwi_restore(x);
        return ERR_MUTEX_NONEED_UNLOCK;
    }
    if(mutex->attr == MUTEX_ATTR_NONBLK)
    {
        mutex->val = 0;
        unlock(&(mutex->lk));
        hwi_restore(x);
        return ERR_NONE;
    }
    mutex->val = 0;
    mln_list_t* p_node = mln_list_head(&(mutex->_list));
    if(p_node == NULL)
    {
        unlock(&(mutex->lk));
        hwi_restore(x);
        return ERR_NONE;
    }
    proc_t* proc = mln_container_of(p_node, proc_t, mutex_node);
    ASSERT(proc->state == PROC_PENDING);
    proc->state &= (~PROC_PENDING);
    proc->state |= (PROC_READY);
    mln_list_node_del(&(mutex->_list), p_node);
    proc->on_mutex = 1;
    sched_add_in(proc);
    hwi_restore(x);
    return ERR_NONE;
}
LOS_ERR_T mutex_init(mutex_t* mutex, mx_attr_t attr)
{
    if(mutex == NULL)
    {
        return ERR_PTR_NULL;
    }
    if(!(attr == MUTEX_ATTR_BLOCK || attr == MUTEX_ATTR_NONBLK))
    {
        return ERR_MUTEX_INFO_ERR;
    }
    mutex->obj_type = LOS_MUTEX;
    mutex->attr = attr;
    mutex->val = 0;
    init_lock(&(mutex->lk),"mutex");
    (mutex->_list).prev = NULL;
    (mutex->_list).next = NULL;
    return ERR_NONE;
}