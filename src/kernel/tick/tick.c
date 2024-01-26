#include "Los_type.h"
#include "Los_err.h"
#include "platform.h"
#include "mln_list.h"
#include "stdarg.h"
#include "string.h"
#include "uart.h"
#include "riscv.h"
#include "assert.h"
#include "spinlock.h"
//上面是依赖 [依赖哪些公共库和哪些模块]
//下面是实现 [实现了哪些公共库或者哪些模块]
#include "Los_timer.h"
#include "tick.h"


static tick_t tick_total;
// 从开机到目前已经经历了多少个tick

static tick_list_t tick_list_handler;
// tick 睡眠链表结构体

static uint8  nest_tick_list;

// be careful : 这里我想要的设计是让 核号为3 的CPU单独处理tick时间相关的函数
// 同时我打算把核号为3的CPU运行在最高状态M态 , 因为lock 只会锁S态的中断 并不能锁M态
// 此时TICK 中断还是开启的 处理链表的过程中并不影响时钟计数
int tick_list_update()
{
    ASSERT(tick_list_handler.obj_type == LOS_TICK_LIST);
    int ret = 0;
    lock(&(tick_list_handler.tick_lock));
    mln_list_t* p_node = mln_list_head(&(tick_list_handler._list));
    while(p_node != NULL)
    {
        mln_list_t* next = mln_list_next(p_node);
        tick_node_t* node = mln_container_of(p_node,tick_node_t,_node);
        ASSERT(node != NULL && node->obj_type == LOS_TICK_NODE);
        ASSERT(node->time_out_handler != NULL);
        ASSERT(node->init_tick != 0);
        node->sleep_tick--;
        if(node->sleep_tick == 0)
        {
            mln_list_node_del(&(tick_list_handler._list),p_node);
            tick_list_handler.num_slping--;   
            if(node->time_out_handler(node) != 0)
            {
                node->sleep_tick = node->init_tick;
                mln_list_add(&(tick_list_handler._list),p_node);
                tick_list_handler.num_slping++;
                if(node->entry_hook != NULL)
                {
                    node->entry_hook(node);
                }
            }
            else
            {
                ret++;
                if(node->exit_hook != NULL)
                {
                    node->exit_hook(node);
                }
            }
        }
        p_node = next;
    }
    unlock(&(tick_list_handler.tick_lock));
    return ret;
}

static inline void tick_list_init(tick_list_t* tk_list)
{
    ASSERT(tk_list != NULL);
    tk_list->obj_type = LOS_TICK_LIST;
    tk_list->num_slping = 0;
    init_lock(&(tk_list->tick_lock),"tick_list_lock");
    (tk_list->_list).prev = NULL;
    (tk_list->_list).next = NULL;
}

tick_t tick_update()
{
    tick_t ret = tick_total++;
    return ret;
}

void tick_init(void)
{
    tick_total = 0;
    tick_list_init(&tick_list_handler);
    nest_tick_list = 0;
}

uint64 tick_to_ms(tick_t tick)
{
    return (uint64)(tick_per_ms()*tick);
}

uint64 tick_to_us(tick_t tick)
{
    return (uint64)(tick_per_ms()*tick*1000);
}


void tick_node_init(tick_node_t* tick_node, tick_no_t handler, tick_t tick, tick_hook_t entry_hook, void* entry_param, tick_hook_t exit_hook, void* exit_param)
{
    ASSERT(tick_node != NULL && handler != NULL && tick != 0);
    tick_node->obj_type = LOS_TICK_NODE;
    tick_node->sleep_tick = tick;
    tick_node->init_tick = tick;
    tick_node->time_out_handler = handler;
    tick_node->entry_hook = entry_hook;
    tick_node->entry_param = entry_param;
    tick_node->exit_hook = exit_hook;
    tick_node->exit_param = exit_param;
    (tick_node->_node).prev = NULL;
    (tick_node->_node).next = NULL;
}

void tick_node_hang(tick_node_t* tick_node)
{
    ASSERT(tick_node != NULL && tick_node->obj_type == LOS_TICK_NODE);
    ASSERT(tick_node->init_tick == tick_node->sleep_tick && tick_node->init_tick != 0);
    ASSERT(tick_node->time_out_handler != NULL);
    ASSERT(tick_list_handler.obj_type == LOS_TICK_LIST);
    lock(&(tick_list_handler.tick_lock));
    mln_list_add(&(tick_list_handler._list),&(tick_node->_node));
    tick_list_handler.num_slping++;
    unlock(&(tick_list_handler.tick_lock));
}

void tick_node_down(tick_node_t* tick_node)
{
    ASSERT(tick_node != NULL && tick_node->obj_type == LOS_TICK_NODE);
    ASSERT(tick_node->init_tick != 0 );
    ASSERT(tick_node->time_out_handler != NULL);
    ASSERT(tick_list_handler.obj_type == LOS_TICK_LIST);
    lock(&(tick_list_handler.tick_lock));
    mln_list_node_del(&(tick_list_handler._list),&(tick_node->_node));
    unlock(&(tick_list_handler.tick_lock));
    return;
}

void set_nest_tk_list()
{
    ASSERT(nest_tick_list == 0); //不允许嵌套 set_nest_tk_list
    nest_tick_list = 1;
}

void unset_nest_tk_list()
{
    ASSERT(nest_tick_list == 1); //不允许嵌套 set_nest_tk_list
    nest_tick_list = 0;
}

bool is_in_nest()
{
    return (nest_tick_list == 1);
}

// 下面是 timer相关的内容

#define MAX_TIMER_SIZE  128


typedef struct timer_inner_s {
    los_type_t      obj_type;
    timer_attr_t    attr;
    timer_handler_t handler;
    void*           param;
    timer_t         timer_id;
    tick_node_t     tk_node;
} timer_inner_t;

timer_inner_t timers[MAX_TIMER_SIZE];
uint32 free_list[MAX_TIMER_SIZE];
uint32 head;
uint32 tail;
uint32 timer_free_num;
uint8  timer_handling; // 为什么我们需要这个
//当然是防刁民
//你不知道用户注册进去的定时器函数到底是什么
//在定时器处理的过程中，会持有 tick链表的锁
//我们不能够有任何的操作尝试去持有这把锁
//也不能调用任何可能去持有锁的函数 
//如 tick_node_hang 和 tick_node_down
//而 port to user 的接口中 
//启动定时器 和 释放定时器都会调用

spinlock_t timer_lock;

static void timer_free_only_list_no_lock(timer_t timer_id);

static void timer_exit_hook(tick_node_t* tick_node);

static timer_inner_t* timer_alloc(timer_t * sd_id);


static int timer_tick_entry(tick_node_t* tick_node);


static void timer_exit_hook(tick_node_t* tick_node)
{
    timer_inner_t* timer = mln_container_of(tick_node, timer_inner_t, tk_node);
    ASSERT(timer->obj_type == LOS_TIMER);
    lock(&timer_lock);
    timer_free_only_list_no_lock(timer->timer_id);
    unlock(&timer_lock);
}

void timer_init(void)
{
    for(int i=0;i<MAX_TIMER_SIZE;i++)
    {
        if(i != MAX_TIMER_SIZE-1)
        {
            free_list[i] = i+1;
        }
        else
        {
            free_list[i] = i;
        }
        timers[i].obj_type = LOS_TIMER;
        timers[i].attr = 0;
        timers[i].handler = NULL;
        timers[i].param = 0;
        timers[i].timer_id = i;
        tick_node_init(&(timers[i].tk_node), timer_tick_entry, 10, NULL, NULL, timer_exit_hook, NULL);
    }
    timer_handling = 0;
    head = 0;
    tail = MAX_TIMER_SIZE-1;
    timer_free_num = MAX_TIMER_SIZE;
    init_lock(&timer_lock, "timer lock");
}

static timer_inner_t* timer_alloc(timer_t * sd_id)
{
    ASSERT(sd_id != NULL);
    timer_inner_t* ret;
    lock(&timer_lock);
    if(timer_free_num <=0)
    {
        unlock(&timer_lock);
        return NULL;
    }
    timer_t tid = head;
    ASSERT(head != 0xffffffff && tail != 0xffffffff);
    ret = &(timers[tid]);
    head = free_list[head];
    free_list[tid] = 0xffffffff;
    timer_free_num--;
    if(timer_free_num ==0)
    {
        head = 0xffffffff;
        tail = 0xffffffff;
    }
    unlock(&timer_lock);
    *sd_id = tid;
    return ret;
}

static void timer_free_only_list_no_lock(timer_t timer_id)
{
    ASSERT(timer_id >=0 && timer_id < MAX_TIMER_SIZE);
    if(free_list[timer_id] != 0xffffffff)
    {
        return;
    }
    if(timer_free_num == 0)
    {
        ASSERT(head == 0xffffffff && tail == 0xffffffff);
        head = timer_id;
        tail = timer_id;
        free_list[head] = timer_id;
    }
    else
    {
        ASSERT(free_list[tail] == tail);
        free_list[tail] = timer_id;
        tail = timer_id;
        free_list[tail] = timer_id;
    }
    timer_free_num++;
    timers[timer_id].attr = 0;
    timers[timer_id].handler = 0;
    timers[timer_id].param = 0;
}

static int timer_tick_entry(tick_node_t* tick_node)
{
    timer_inner_t* timer = mln_container_of(tick_node,timer_inner_t,tk_node);
    ASSERT(timer->obj_type == LOS_TIMER);
    ASSERT(timer->handler != NULL);
    lock(&timer_lock);
    timer_handling++;
    unlock(&timer_lock);
    timer->handler(timer->param);
    lock(&timer_lock);
    timer_handling--;
    unlock(&timer_lock);
    if(timer->attr & TIMER_ONCE)
    {
        return 0;
    }
    return -1;
}


LOS_ERR_T timer_register(timer_t* timer_id, const timer_info_t* timer_info)
{
    if(timer_id == NULL || timer_info == NULL)
    {
        return ERR_PTR_NULL;
    }
    if(timer_info->handler == NULL)
    {
        return ERR_PTR_NULL;
    }
    if((((timer_info->attr)&TIMER_ONCE)==0) && (((timer_info->attr)&TIMER_ALWAYS)==0))
    {
        return ERR_TIMER_INFO_ERR;
    }
    if((((timer_info->attr)&TIMER_ONCE)!=0) && (((timer_info->attr)&TIMER_ALWAYS)!=0))
    {
        return ERR_TIMER_INFO_ERR;
    }
    if(timer_info->tick ==0)
    {
        return ERR_TIMER_INFO_ERR;
    }
    timer_inner_t* timer = timer_alloc(timer_id);
    if(timer == NULL)
    {
        return ERR_MEMORY_EMPTY;
    }
    timer->attr = timer_info->attr;
    timer->param = timer_info->param;
    timer->handler = timer_info->handler;
    tick_node_init(&(timer->tk_node), timer_tick_entry, timer_info->tick, NULL, NULL, timer_exit_hook, NULL);
    lock(&timer_lock);
    if(timer_handling != 0)
    {
        timer_free_only_list_no_lock(*timer_id);
        unlock(&timer_lock);
        return ERR_TIMER_HANDLING;
    }
    unlock(&timer_lock);
    tick_node_hang(&(timer->tk_node));
    return ERR_NONE;
}
LOS_ERR_T timer_unregister(timer_t timer_id)
{
    if(!(timer_id>=0 && timer_id < MAX_TIMER_SIZE))
    {
        return ERR_TIMER_INFO_ERR;
    }
    lock(&timer_lock);
    if(free_list[timer_id] != 0xffffffff)
    {
        unlock(&timer_lock);
        return ERR_TIMER_INFO_ERR;
    }
    if(timer_handling !=0)
    {
        unlock(&timer_lock);
        return ERR_TIMER_HANDLING;
    }
    unlock(&timer_lock);
    tick_node_down(&(timers[timer_id].tk_node));
    lock(&timer_lock);
    timer_free_only_list_no_lock(timer_id);
    unlock(&timer_lock);
    return ERR_NONE;
}