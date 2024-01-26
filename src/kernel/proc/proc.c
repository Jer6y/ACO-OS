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
//上面是依赖 [依赖哪些公共库和哪些模块]
//下面是实现 [实现了哪些公共库或者哪些模块]
#include "proc.h"
#include "Los_signal.h"


#define PROC_STACK_MIN_SIZE 1024
#define PROC_STACK_DYN_SIZE 4096


#define IDLE_STACK_SIZE  1024
static proc_t idle_handle[CPUS];
uint8  idle_stk[CPUS][IDLE_STACK_SIZE];

struct {
    spinlock_t zombie_lock;
    mln_list_t _list;
    // for debug
    int zombie_num;
} zombie_list;

void idle_task(void * prama)
{
    int i = 0;
    while(1)
    {
        printk("[%d cpu] : %p\n",r_tp(),RUNNING_PROC);
        asm volatile("wfi");
    }
}

proc_t* running_proc[CPUS];

typedef struct allocator_s {
    los_type_t obj_type;
    mln_list_t _list;
    int        obj_total;
    spinlock_t lock;
} allocator_t;

allocator_t allocator_proc;
static proc_t* proc_alloc()
{
    ASSERT(allocator_proc.obj_type == LOS_PROC_ALLOCATOR);
    ASSERT(allocator_proc.obj_total >= 0);
    if(allocator_proc.obj_total == 0)
    {
        return NULL;
    }
    void* ret = 0;
    mln_list_t* p_node = mln_list_head(&(allocator_proc._list));
    while(p_node != NULL)
    {
        m_slab_t* node = mln_container_of(p_node,m_slab_t,_node);
        ret = m_slab_obj_alloc(node);
        if(ret != NULL)
        {
            break;
        }
        p_node = mln_list_next(p_node);
    }
    if(ret != (void*)0)
    {
        lock(&(allocator_proc.lock));
        allocator_proc.obj_total--;
        unlock(&(allocator_proc.lock));
    }
    return (proc_t*)ret;
}

static void proc_free(proc_t* proc)
{
    ASSERT(allocator_proc.obj_type == LOS_PROC_ALLOCATOR);
    ASSERT(allocator_proc.obj_total >= 0);
    mln_list_t* p_node = mln_list_head(&(allocator_proc._list));
    while(p_node != NULL)
    {
        m_slab_t* node = mln_container_of(p_node,m_slab_t,_node);
        uint64 left = (uint64)m_slab_get_r_start(node);
        uint64 right = (uint64)m_slab_get_r_end(node);
        if((uint64)proc > left && (uint64)proc < right )
        {
            m_slab_obj_free(node,(void*)proc);
            proc = NULL;
            break;
        }
        p_node = mln_list_next(p_node);
    }
    if(proc == NULL)
    {
        lock(&(allocator_proc.lock));
        allocator_proc.obj_total++;
        unlock(&(allocator_proc.lock));
    }
    return;
}

void entry_text()
{
    RUNNING_PROC->text_entry(RUNNING_PROC->param);
    proc_kill(0);
}

static uint64* init_stk(uint64 stk_base, uint64 stk_size)
{
    ASSERT(stk_size > sizeof(riscv_contex_t));
    uint64 stk_sp = stk_base + stk_size;
    stk_sp &= (~0b1111); //16字节对齐
    stk_sp -= sizeof(riscv_contex_t);
    riscv_contex_t* contex = (riscv_contex_t*)stk_sp;
    contex->ra = 0x1;
    contex->gp = 0x2;
    contex->tp = r_tp();
    contex->t0 = 0x3;
    contex->t1 = 0x4;
    contex->t2 = 0x5;
    contex->s0 = 0x6;
    contex->s1 = 0x7;
    contex->a0 = 0x8;
    contex->a1 = 0x9;
    contex->a2 = 0x10;
    contex->a3 = 0x11;
    contex->a4 = 0x12;
    contex->a5 = 0x13;
    contex->a6 = 0x14;
    contex->a7 = 0x15;
    contex->s2 = 0x17;
    contex->s3 = 0x18;
    contex->s4 = 0x19;
    contex->s5 = 0x20;
    contex->s6 = 0x21;
    contex->s7 = 0x22;
    contex->s8 = 0x23;
    contex->s9 = 0x24;
    contex->s10 = 0x25;
    contex->s11 = 0x26;
    contex->t3 = 0x27;
    contex->t4 = 0x28;
    contex->t5 = 0x29;
    contex->t6 = 0x30;
    contex->sepc = (uint64)entry_text;
    contex->sstatus = (r_sstatus() | SPP_S | SPIE_S);
    return (uint64*)stk_sp;
}

void proc_init()
{
    allocator_proc.obj_type = LOS_PROC_ALLOCATOR;
    allocator_proc._list.prev = NULL;
    allocator_proc._list.next = NULL;
    allocator_proc.obj_total = 0;
    init_lock(&(allocator_proc.lock),"proc allocator lock");
    for(int i=0;i<10; i++)
    {
        m_slab_t* tmp_slab;
        void* ptr = m_pool_alloc(&mm_pool,5);
        ASSERT(ptr != NULL);
        tmp_slab = m_slab_init_align(ptr, 5, sizeof(proc_t), 4);
        mln_list_add(&(allocator_proc._list),&(tmp_slab->_node));
        allocator_proc.obj_total += m_slab_obj_rest(tmp_slab);
    }
    for(int i=0;i<CPUS;i++)
    {
        proc_info_t info;
        running_proc[i] = NULL;
        info.stack_base = (uint64*)&(idle_stk[i][0]);
        info.stack_size = IDLE_STACK_SIZE;
        info.tex_entry = idle_task;
        info.param = (void*)(0x2211);
        info.prio = MAX_TASK_PRIO-1;
        ASSERT(proc_create(&info,&(idle_handle[i]))== ERR_NONE);
    }
    init_lock(&(zombie_list.zombie_lock),"zombie lk");
    zombie_list.zombie_num = 0;
    (zombie_list._list).prev = NULL;
    (zombie_list._list).next = NULL;
}



//这个是释放proc 资源的
static void proc_release_and_free(proc_t* proc)
{
    ASSERT(proc != NULL && proc->obj_type == LOS_PROC);
    if(proc->alloc_flag == PROC_ALLOC_SYS)
    {
        if(proc->stack_base != NULL)
        {
            m_pool_free(&mm_pool,(void*)(proc->stack_base),1);
            proc->stack_base = NULL;
        }
        // 这会清理掉proc的所有内容 
        proc_free(proc);
    }
    else
    {
        // 直接清除obj_type 就好 不用全部清了
        // 魔术字带来的优化
        proc->obj_type = 0;
    }
}

void zombie_update()
{
    lock(&(zombie_list.zombie_lock));
    mln_list_t* p_node = mln_list_head(&(zombie_list._list));
    while(p_node != NULL)
    {
        mln_list_t* next = mln_list_next(p_node);
        proc_t* node = mln_container_of(p_node, proc_t, zombie_node);
        mln_list_node_del(&(zombie_list._list),p_node);
        zombie_list.zombie_num--;
        ASSERT(node->obj_type == LOS_PROC);
        ASSERT((node->state & PROC_ZOMBIE) !=0);
        ASSERT((node->state & (~PROC_ZOMBIE)) ==0);
        proc_release_and_free(node);
        p_node = next;
    }
    unlock(&(zombie_list.zombie_lock));
}
void proc_on_zombie(proc_t* proc)
{
    ASSERT(proc != NULL && proc->obj_type == LOS_PROC);
    lock(&(zombie_list.zombie_lock));
    mln_list_add(&(zombie_list._list),&(proc->zombie_node));
    zombie_list.zombie_num++;
    unlock(&(zombie_list.zombie_lock));
}
static void proc_init_static(proc_t* proc, prio_t prio, pexc_entry entry, void* param)
{
    ASSERT(proc != NULL && entry != NULL);
    proc->obj_type = LOS_PROC;
    proc->state = PROC_READY;
    proc->prio = prio;
    proc->text_entry = entry;
    proc->param = param;
    proc->signal = 0;
    (proc->ready_node).next = NULL;
    (proc->ready_node).prev = NULL;
    (proc->pend_node).next = NULL;
    (proc->pend_node).prev = NULL;
    (proc->time_node).next = NULL;
    (proc->time_node).prev = NULL;
    (proc->sem_node).next = NULL;
    (proc->sem_node).prev = NULL;
    (proc->zombie_node).next = NULL;
    (proc->zombie_node).prev = NULL;
    init_lock(&(proc->proc_lock),"proc lock");
    proc->stack_base = NULL;
    proc->stack_size = 0;
}
static void proc_init_stk(proc_t* proc, uint64* stack_base, uint64 stack_size)
{
    ASSERT(proc != NULL && stack_base != NULL);
    ASSERT(stack_size >0);
    proc->stack_base = stack_base;
    proc->stack_size = stack_size;
}

LOS_ERR_T proc_create(proc_info_t* info, proc_t* proc_sd_in)
{
    if(info == NULL || proc_sd_in == NULL)
    {
        return ERR_PTR_NULL;
    }
    if(info->tex_entry == NULL || info->stack_base == NULL || info->stack_size < PROC_STACK_MIN_SIZE)
    {
        return ERR_PROC_INFO_ERR;
    }
    if(TASK_PRIO_ILLEAGEL(info->prio))
    {
        return ERR_PROC_INFO_ERR;
    }
    proc_sd_in->alloc_flag = PROC_ALLOC_USR;
    proc_init_static(proc_sd_in, info->prio, info->tex_entry, info->param);
    proc_init_stk(proc_sd_in, info->stack_base, info->stack_size);
    proc_sd_in->sp = init_stk((uint64)proc_sd_in->stack_base, proc_sd_in->stack_size);
    sched_add_in(proc_sd_in);
    return ERR_NONE;
}


LOS_ERR_T proc_create_dyn(proc_t** proc_sd_in, prio_t prio, pexc_entry entry, void* param)
{
    if(proc_sd_in == NULL || entry == NULL)
    {
        return ERR_PTR_NULL;
    }
    if(TASK_PRIO_ILLEAGEL(prio))
    {
        return ERR_PROC_INFO_ERR;
    }
    proc_t* proc = proc_alloc();
    if(proc == NULL )
    {
        return ERR_MEMORY_EMPTY;
    }
    proc->alloc_flag = PROC_ALLOC_SYS;
    proc_init_static(proc, prio, entry, param);
    uint64 stk_size = PROC_STACK_DYN_SIZE;
    uint64* stk_base = m_pool_alloc(&mm_pool,1);
    if(stk_base == NULL)
    {
        proc_release_and_free(proc);
        return ERR_MEMORY_EMPTY;
    }
    proc_init_stk(proc, stk_base, stk_size);
    proc->sp = init_stk((uint64)stk_base,stk_size);
    *proc_sd_in = proc;
    sched_add_in(proc);
    return ERR_NONE;
}

LOS_ERR_T proc_kill(proc_t* proc)
{
    if(proc == NULL)
    {
        proc = RUNNING_PROC;
    }
    if(proc->obj_type != LOS_PROC)
    {
        return ERR_OBJ_MAGIC_ERR;
    }
    signal_proc(proc, SIG_KILL);
    if(proc == RUNNING_PROC)
    {
        tsk_trap(ID_SCHED,NULL);
    }
    return ERR_NONE;
}
LOS_ERR_T proc_suspend(proc_t* proc)
{
    return ERR_PTR_NULL;
}
LOS_ERR_T proc_resume(proc_t* proc)
{
    return ERR_PTR_NULL;
}
LOS_ERR_T proc_delay(proc_t* proc)
{
    if(proc == NULL)
    {
        return ERR_PTR_NULL;
    }
    if(proc->obj_type != LOS_PROC)
    {
        return ERR_OBJ_MAGIC_ERR;
    }
    return ERR_PTR_NULL;
}


void dump_proc_mem()
{
    ASSERT(allocator_proc.obj_type == LOS_PROC_ALLOCATOR);
    ASSERT(allocator_proc.obj_total >= 0);
    printk("obj_total:%d\n",allocator_proc.obj_total);
    mln_list_t* p_node = mln_list_head(&(allocator_proc._list));
    int i =0;
    while(p_node != NULL)
    {
        m_slab_t* node = mln_container_of(p_node,m_slab_t,_node);
        int ret = m_slab_obj_rest(node);
        int all = m_slab_obj_all(node);
        printk("%d slab_allocator : all node %d rest_node %d\n",i++,all,ret);
        p_node = mln_list_next(p_node);
    }
}