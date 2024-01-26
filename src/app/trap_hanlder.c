#include "Los.h"
#include "proc.h"
#include "sched.h"
#define IS_INTERRUPT(x) (((x)&0x8000000000000000ull) != 0)
#define IS_SOFT_INT(x)  ((x) == 0x8000000000000001ull)

void clear_soft_pending()
{
    uint64 x = r_sip();
    x &= SSIP_R;
    w_sip(x);
}

void trap_handler(uint64 scause, uint64 stval, uint64 tsk_trap_id, void* param)
{
    if(IS_INTERRUPT(scause))
    {
        if(IS_SOFT_INT(scause))
        {
            clear_soft_pending();
            SET_SCHED();
        }
        else 
        {
            
        }
    }
    else 
    {
        if(tsk_trap_id == ID_SCHED)
        {
            riscv_contex_t* contex = (riscv_contex_t*)(RUNNING_PROC->sp);
            contex->sepc +=4;
            if(scheduler_locked())
            {
                contex->a0 = 0x65791;
                return;
            }
            SET_SCHED();
            return;
        }
        else if(tsk_trap_id == ID_MUTEX)
        {
            riscv_contex_t* contex = (riscv_contex_t*)(RUNNING_PROC->sp);
            contex->sepc +=4;
            if(scheduler_locked())
            {
                contex->a0 = 0x65791;
                return;
            }
            mutex_t* mutex = (mutex_t*)param;
            ASSERT(RUNNING_PROC->state == PROC_RUNNING);
            RUNNING_PROC->state &= (~PROC_RUNNING);
            RUNNING_PROC->state |= PROC_PENDING;
            mln_list_add(&(mutex->_list),&(RUNNING_PROC->mutex_node));
            //这里有点trick的 , 所有mutex 之前必须 调用hwi_lock(), 也就是说
            //spinlock 里面存的 intSave 永远是0 所以释放掉也不怕在trap中打开中断
            unlock(&(mutex->lk));
            RUNNING_PROC = sched_get_highest();
            ASSERT(RUNNING_PROC->state == PROC_READY);
            RUNNING_PROC->state &= (~PROC_READY);
            RUNNING_PROC->state |= PROC_RUNNING;
            SET_SCHED();
            return;
        }
        else if(tsk_trap_id == ID_SEM)
        {
            riscv_contex_t* contex = (riscv_contex_t*)(RUNNING_PROC->sp);
            contex->sepc +=4;
            if(scheduler_locked())
            {
                contex->a0 = 0x65791;
                return;
            }
            semphare_t* sem = (semphare_t*)param;
            ASSERT(RUNNING_PROC->state == PROC_RUNNING);
            RUNNING_PROC->state &= (~PROC_RUNNING);
            RUNNING_PROC->state |= PROC_PENDING;
            mln_list_add(&(sem->_list),&(RUNNING_PROC->sem_node));
            unlock(&(sem->lk));
            RUNNING_PROC = sched_get_highest();
            ASSERT(RUNNING_PROC->state == PROC_READY);
            RUNNING_PROC->state &= (~PROC_READY);
            RUNNING_PROC->state |= PROC_RUNNING;
            SET_SCHED();
            return;
        }

        panic("happen exception");
    }
    return;
}