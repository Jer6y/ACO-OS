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
#include "Los_semphare.h"



LOS_ERR_T sem_post(semphare_t* sem)
{
    if(sem == NULL || sem->obj_type != LOS_SEM)
    {
        return ERR_SEM_INFO_ERR;
    }
    uint64 x = hwi_lock();
    lock(&(sem->lk));
    if(sem->val >=0)
    {
        sem->val++;
        unlock(&(sem->lk));
        hwi_restore(x);
        return ERR_NONE;
    }
    sem->val++;
    mln_list_t* p_node = mln_list_head(&(sem->_list));
    ASSERT(p_node != NULL);
    proc_t* proc = mln_container_of(p_node,proc_t,sem_node);
    ASSERT(proc->state == PROC_PENDING);
    proc->state &= (~PROC_PENDING);
    proc->state |= (PROC_READY);
    mln_list_node_del(&(sem->_list), p_node);
    proc->on_sem = 1;
    sched_add_in(proc);
    hwi_restore(x);
    return ERR_NONE;
}

LOS_ERR_T sem_wait(semphare_t* sem)
{
    if(sem == NULL || sem->obj_type != LOS_SEM)
    {
        return ERR_SEM_INFO_ERR;
    }
    uint64 x = hwi_lock();
    lock(&(sem->lk));
    if(sem->val >0)
    {
        sem->val--;
        unlock(&(sem->lk));
        hwi_restore(x);
        return ERR_NONE;
    }
    sem->val--;
    if(tsk_trap(ID_SEM, sem) == 0x65791)
    {
        unlock(&(sem->lk));
        hwi_restore(x);
        return ERR_SCHED_LOCKED;
    }

    //
    RUNNING_PROC->on_sem =0;
    unlock(&(sem->lk));
    hwi_restore(x);
    return ERR_NONE;
}

LOS_ERR_T sem_init(semphare_t* sem, int val)
{
    if(sem == NULL || val <0)
    {
        return ERR_SEM_INFO_ERR;
    }    
    sem->obj_type = LOS_SEM;
    sem->val = val;
    init_lock(&(sem->lk),"sem");
    (sem->_list).next = NULL;
    (sem->_list).prev = NULL;
}