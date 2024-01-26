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


void signal_handler(proc_t* proc)
{
    ASSERT(proc != NULL && proc->obj_type == LOS_PROC);
    lock(&(proc->proc_lock));
    if(((proc->signal) & SIG_KILL) != 0)
    {
        proc->signal = 0;
        ASSERT((proc->state & PROC_RUNNING)!=0);
        ASSERT((proc->state & (~PROC_RUNNING))==0);
        proc->state &= ~(PROC_RUNNING);
        proc->state |= (PROC_ZOMBIE);
        proc_on_zombie(proc);
        unlock(&(proc->proc_lock));
        RUNNING_PROC = sched_get_highest();
        ASSERT((RUNNING_PROC->state & PROC_READY)!=0);
        ASSERT((RUNNING_PROC->state & (~PROC_READY))==0);
        RUNNING_PROC->state &= ~(PROC_READY);
        RUNNING_PROC->state |= PROC_RUNNING;
        return;
    }
    unlock(&(proc->proc_lock));
    return;
}


LOS_ERR_T designal_proc(proc_t* proc, sig_t signal)
{
    if(proc == NULL)
    {
        return ERR_PTR_NULL;
    }
    if(proc->obj_type != LOS_PROC)
    {
        return ERR_OBJ_MAGIC_ERR;
    }
    lock(&(proc->proc_lock));
    proc->signal &= (~signal);
    unlock(&(proc->proc_lock));
    return ERR_NONE;
}

LOS_ERR_T signal_proc(proc_t* proc, sig_t signal)
{
    if(proc == NULL)
    {
        return ERR_PTR_NULL;
    }
    if(proc->obj_type != LOS_PROC)
    {
        return ERR_OBJ_MAGIC_ERR;
    }
    lock(&(proc->proc_lock));
    proc->signal |= signal;
    unlock(&(proc->proc_lock));
    return ERR_NONE;
}