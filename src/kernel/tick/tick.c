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
            }
            else
            {
                ret++;
            }
        }
        p_node = mln_list_next(p_node);
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


void tick_node_init(tick_node_t* tick_node, tick_no_t handler, tick_t tick)
{
    ASSERT(tick_node != NULL && handler != NULL && tick != 0);
    tick_node->obj_type = LOS_TICK_NODE;
    tick_node->sleep_tick = tick;
    tick_node->init_tick = tick;
    tick_node->time_out_handler = handler;
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