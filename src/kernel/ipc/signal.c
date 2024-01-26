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


void delay_handler(void* param)
{
    proc_t* proc = (void*)param;
    ASSERT(proc->state == PROC_ON_WAIT);
    proc_drop_pend(proc);
    lock(&(proc->proc_lock));
    proc->signal = 0;
    unlock(&(proc->proc_lock));
    proc->state &= (~PROC_ON_WAIT);
    proc->state |= PROC_READY;
    sched_add_in(proc);
    return;
}

int signal_handler_in_scheduler(proc_t* proc)
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
        unlock(&(proc->proc_lock));
        proc_on_zombie(proc);
        RUNNING_PROC = sched_get_highest();
        ASSERT((RUNNING_PROC->state & PROC_READY)!=0);
        ASSERT((RUNNING_PROC->state & (~PROC_READY))==0);
        RUNNING_PROC->state &= ~(PROC_READY);
        RUNNING_PROC->state |= PROC_RUNNING;
        return -1;
    }
    if(((proc->signal) & SIG_RESUME) != 0)
    {
        proc->signal = 0;
        unlock(&(proc->proc_lock));    
        return 0;
    }
    if(((proc->signal) & SIG_PEND) != 0)
    {
        proc->signal = 0;
        ASSERT((proc->state & PROC_RUNNING)!=0);
        ASSERT((proc->state & (~PROC_RUNNING))==0);
        proc->state &= ~(PROC_RUNNING);
        proc->state |= (PROC_PENDING);
        unlock(&(proc->proc_lock));
        proc_on_pend(proc);
        RUNNING_PROC = sched_get_highest();
        ASSERT((RUNNING_PROC->state & PROC_READY)!=0);
        ASSERT((RUNNING_PROC->state & (~PROC_READY))==0);
        RUNNING_PROC->state &= ~(PROC_READY);
        RUNNING_PROC->state |= PROC_RUNNING;
        return -1;
    }
    if(((proc->signal) & SIG_DELAY) != 0)
    {
        proc->signal = 0;
        ASSERT((proc->state & PROC_RUNNING)!=0);
        ASSERT((proc->state & (~PROC_RUNNING))==0);
        proc->state &= ~(PROC_RUNNING);
        proc->state |= (PROC_ON_WAIT);
        unlock(&(proc->proc_lock));
        timer_t tid;
        timer_info_t info;
        info.attr = TIMER_ONCE;
        info.handler = delay_handler;
        info.tick = 10;
        info.param = proc;
        if(timer_register(&tid, &info) != ERR_NONE)
        {
            return 0;    
        }
        proc_on_pend(proc);
        RUNNING_PROC = sched_get_highest();
        ASSERT((RUNNING_PROC->state & PROC_READY)!=0);
        ASSERT((RUNNING_PROC->state & (~PROC_READY))==0);
        RUNNING_PROC->state &= ~(PROC_READY);
        RUNNING_PROC->state |= PROC_RUNNING;
        return -1;
    }
    proc->signal = 0;
    unlock(&(proc->proc_lock));
    return 0;
}

//返回-1 还是应该on pend
//返回0  解放了
int signal_handler_on_pend(proc_t* proc)
{
    ASSERT(proc != NULL && proc->obj_type == LOS_PROC);
    lock(&(proc->proc_lock));
    if(((proc->signal) & SIG_KILL) != 0)
    {
        proc->signal = 0;
        ASSERT((proc->state & PROC_PENDING)!=0);
        ASSERT((proc->state & (~PROC_PENDING))==0);
        proc->state &= ~(PROC_PENDING);
        proc->state |= (PROC_ZOMBIE);
        unlock(&(proc->proc_lock));
        proc_on_zombie(proc);
        return 0;
    }
    if(((proc->signal) & SIG_RESUME) != 0)
    {
        proc->signal = 0;
        ASSERT((proc->state & PROC_PENDING)!=0);
        ASSERT((proc->state & (~PROC_PENDING))==0);
        proc->state &= ~(PROC_PENDING);
        proc->state |= (PROC_READY);
        unlock(&(proc->proc_lock));
        sched_add_in(proc);
        return 0;
    }
    proc->signal = 0;
    unlock(&(proc->proc_lock));
    return -1;
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