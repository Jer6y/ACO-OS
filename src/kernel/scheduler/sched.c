#include "Los_type.h"
#include "Los_err.h"
#include "platform.h"
#include "stdarg.h"
#include "string.h"
#include "riscv.h"
#include "uart.h"
#include "plic.h"
#include "clint.h"
#include "spinlock.h"
#include "assert.h"
#include "memory.h"
#include "proc.h"
//上面是依赖 [依赖哪些公共库和哪些模块]
//下面是实现 [实现了哪些公共库或者哪些模块]
#include "sched.h"

extern void idle_task(void * prama);
extern void tsk_contex_load(proc_t* proc);
//======================不需要PORT出去的数据结构===========================
//就绪链表
typedef struct tsk_rdy_list_s {
    los_type_t obj_type;
    spinlock_t rdy_lock;
    mln_list_t _list;
} tsk_rdy_t;

//就绪队列
typedef struct task_rdy_queue_s {
    los_type_t obj_type;
    tsk_rdy_t  rdy_lists[MAX_TASK_PRIO];
    spinlock_t rdy_queue_lock;
    int        prio_highest;
} task_rdy_queue_t;

static task_rdy_queue_t tsk_queue;

static int scheduler_lock[CPUS];
//for 4 cpu scheduler_lock 
//不需要spinlock来维护互斥

//======================================================================


//=======================需要PORT出去的数据结构===========================
// 这里处理的并不好，把中断嵌套的变量交给调度器模块管理
// 因为本人不太想重新建模一个system模块来存储系统状态变量了
// 所以直接交给调度器管理了， 千万不要学习 这种一点都不内聚
int int_nest[CPUS];

// 同理 这也是一个系统状态变量 表示能够调度 两个都是PORT出去给其他内核模块使用的变量
int on_sched[CPUS];

//=======================================================================


bool check_is_in_nest()
{
    return IS_INT_NESTED();
}

static void sched_add_in_nolock(proc_t* proc)
{
    ASSERT(proc != NULL);
    ASSERT(proc->state == PROC_READY);
    prio_t prio = proc->prio;
    mln_list_t* list = &(proc->ready_node);
    if(prio <= tsk_queue.prio_highest)
    {
        tsk_queue.prio_highest = prio;
    }
    mln_list_add(&(tsk_queue.rdy_lists[prio]._list),list);
}

static void tsk_list_init(tsk_rdy_t* rdy_list)
{
    ASSERT(rdy_list != NULL);
    rdy_list->obj_type = LOS_RDY_LIST;
    (rdy_list->_list).prev = NULL;
    (rdy_list->_list).next = NULL;
    init_lock(&(rdy_list->rdy_lock),"rdy list lock");
}

void sched_init()
{
    tsk_queue.obj_type = LOS_RDY_QUEUE;
    for(int i=0;i<MAX_TASK_PRIO;i++)
    {
        tsk_list_init(&(tsk_queue.rdy_lists[i]));
    }
    tsk_queue.prio_highest = 0xffff;
    init_lock(&(tsk_queue.rdy_queue_lock),"rdy queue lock");
    for(int i=0;i<CPUS;i++)
    {
        scheduler_lock[i] = 0;
        int_nest[i] = 0;
        on_sched[i] = 0;
    }
}

static proc_t* sched_get_highest_no_lock()
{
    mln_list_t* ret = NULL;
    ret = mln_list_head(&((tsk_queue.rdy_lists[tsk_queue.prio_highest])._list));
    ASSERT(ret != NULL);
    mln_list_node_del(&((tsk_queue.rdy_lists[tsk_queue.prio_highest])._list),ret);
    mln_list_t* tmp;
    while(1)
    {
        tmp = mln_list_head(&((tsk_queue.rdy_lists[tsk_queue.prio_highest])._list));
        if(tmp != NULL)
        {
            break;
        }
        tsk_queue.prio_highest++;
        ASSERT(tsk_queue.prio_highest < MAX_TASK_PRIO);
    }
    ASSERT(tmp != NULL);
    proc_t* ret_proc = mln_container_of(ret,proc_t,ready_node);
    return ret_proc;
}

//不可能返回NULL 至少让每个CPU能够有程序可以调度
//若没有 则说明应该PANIC
proc_t* sched_get_highest()
{
    ASSERT(tsk_queue.obj_type == LOS_RDY_QUEUE && tsk_queue.prio_highest != -1);
    proc_t* ret;
    lock(&(tsk_queue.rdy_queue_lock));
    ret = sched_get_highest_no_lock();
    unlock(&(tsk_queue.rdy_queue_lock));
    return ret;
}

//不需要CPU之间的锁 只需要CPU内部的临界区
int scheduler_locked()
{
    uint64 x = hwi_lock();
    if(scheduler_lock[r_tp()] ==1)
    {
        hwi_restore(x);
        return 1;
    }   
    hwi_restore(x);
    return 0;
}

int scheduler_unlocked()
{
    uint64 x = hwi_lock();
    if(scheduler_lock[r_tp()] ==1)
    {
        hwi_restore(x);
        return 0;
    }   
    hwi_restore(x);
    return 1;
}

// 锁调度器
LOS_ERR_T lock_scheduler()
{
    uint64 x = hwi_lock();
    if(scheduler_lock[r_tp()] != 0)
    {
        hwi_restore(x);
        return ERR_SCHED_LOCKED;
    }
    scheduler_lock[r_tp()] = 1;
    hwi_restore(x);
}

// 解锁调度器
LOS_ERR_T unlock_scheduler()
{
    uint64 x = hwi_lock();
    if(scheduler_lock[r_tp()] ==0)
    {
        hwi_restore(x);
        return ERR_SCHED_UNLOCKED;
    }
    scheduler_lock[r_tp()] = 0;
    hwi_restore(x);
}

void first_sched()
{
    ASSERT(RUNNING_PROC == NULL);
    proc_t* proc = sched_get_highest();
    RUNNING_PROC = proc;
    ASSERT(RUNNING_PROC->obj_type == LOS_PROC);
    if(((RUNNING_PROC->state) & (PROC_READY)) ==0)
    {
        panic("on ready but not ready task ??");
    }
    if(((RUNNING_PROC->state) & (~(PROC_READY))) !=0)
    {
        panic("except ready , has other state??");
    }
    RUNNING_PROC->state &= ~(PROC_READY);
    RUNNING_PROC->state |= PROC_RUNNING;
    tsk_contex_load(proc);
}

static bool is_need_sched_no_lock()
{
    if(RUNNING_PROC->prio >= tsk_queue.prio_highest)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static proc_t* sched_swtch_highest()
{
    lock(&(tsk_queue.rdy_queue_lock));
    if(!is_need_sched_no_lock())
    {
        RUNNING_PROC->state &= ~(PROC_RUNNING);
        RUNNING_PROC->state |= PROC_READY;
        unlock(&(tsk_queue.rdy_queue_lock));
        return RUNNING_PROC;
    }
    RUNNING_PROC->state &= ~(PROC_RUNNING);
    RUNNING_PROC->state |= PROC_READY;
    sched_add_in_nolock(RUNNING_PROC);
    proc_t* proc = sched_get_highest_no_lock();
    unlock(&(tsk_queue.rdy_queue_lock));
    return proc;
}
extern int signal_handler_in_scheduler(proc_t* proc);
//调度函数 不能在嵌套中断中调用
//不能在任务中调用 只能在软件中断 或者特定的异常处理调用
//必须进入CPU内部临界区 不需要CPU之间的锁 
void sched()
{
    if(IS_INT_NOT_SCHED())
    {
        return;
    }
    //判断是否调度器被锁了
    if(scheduler_lock[r_tp()] ==1)
    {
        SET_UN_SCHED();
        return;
    }
    
    //判断是否有嵌套中断
    if(int_nest[r_tp()] > 0)
    {
        panic("scheduler want but  in int_nest");
    }

    ASSERT(RUNNING_PROC != NULL);
    ASSERT(RUNNING_PROC->obj_type == LOS_PROC);
    ASSERT(((RUNNING_PROC->state) & PROC_RUNNING) !=0);
    ASSERT(((RUNNING_PROC->state) & ~(PROC_RUNNING)) ==0);
    RUNNING_PROC = sched_swtch_highest();
    ASSERT(RUNNING_PROC->obj_type == LOS_PROC);
    if(((RUNNING_PROC->state) & (PROC_READY)) ==0)
    {
        panic("on ready but not ready task ??");
    }
    if(((RUNNING_PROC->state) & (~(PROC_READY))) !=0)
    {
        panic("except ready , has other state??");
    }
    RUNNING_PROC->state &= ~(PROC_READY);
    RUNNING_PROC->state |= PROC_RUNNING;
    SET_UN_SCHED();
    // 这里是给信号signal 留白的 
    while(1)
    {
        if(RUNNING_PROC->text_entry == idle_task || RUNNING_PROC->on_mutex ==1 || RUNNING_PROC->on_sem ==1)
        {
            break;
        }
        if(signal_handler_in_scheduler(RUNNING_PROC) ==0)
        {
            break;
        }
    }
    tsk_contex_load(RUNNING_PROC);
}

void interrupt_in()
{
    int_nest[r_tp()]++;
}
void interrupt_out()
{
    ASSERT(int_nest[r_tp()]>0);
    int_nest[r_tp()]--;
}

// 不是实时的哦！
void sched_add_in(proc_t* proc)
{
    ASSERT(proc != NULL);
    ASSERT(proc->obj_type == LOS_PROC);
    ASSERT(tsk_queue.obj_type == LOS_RDY_QUEUE);
    lock(&(tsk_queue.rdy_queue_lock));
    sched_add_in_nolock(proc);
    unlock(&(tsk_queue.rdy_queue_lock));
}