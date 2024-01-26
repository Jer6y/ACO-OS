/*
 * 这个是os tick 模块软件部分 
 * 依赖于具体硬件实现
 * 主要作用: 维护内部 tick量 和 睡眠队列
 * 这个模块不作为port to user 来使用
 * 作为port to other kernel module 进行使用
 */
#ifndef TICK_H
#define TICK_H

#include "Los_tick.h"
#include "Los_timer.h"

typedef struct tick_list_s  tick_list_t;

typedef struct tick_node_s  tick_node_t;

typedef int (*tick_no_t)(tick_node_t* tick_node);

typedef void (*tick_hook_t)(tick_node_t* tick_node);

typedef struct tick_list_s {
    los_type_t obj_type;
    uint32     num_slping;
    spinlock_t tick_lock;
    mln_list_t _list;    
} tick_list_t;

typedef struct tick_node_s {
    los_type_t obj_type;
    tick_t     sleep_tick;      //目前还剩多少个tick 被唤醒
    tick_t     init_tick;       //初始挂上的时候的tick数目 , 在time_out_handler 返回-1时 会重新加载挂上
                                //可以使用 -1 特性去构造软件定时器
    tick_no_t  time_out_handler;
    
    tick_hook_t entry_hook;     //进入tick 时的hook函数
    void*       entry_param;    //hook param

    tick_hook_t exit_hook;      //退出tick 时的hook函数
    void*       exit_param;     //exit param

    mln_list_t _node;
} tick_node_t;

// tick 模块初始化 只能在OS开启前调用
void tick_init(void);

// 更新一个tick , 返回值为更新前的tick值
// 在硬件tick中断的时候需要调用
tick_t tick_update();

// 更新一次tick 列表
// 返回的值为此次释放的列表的链表数目
int tick_list_update();

//不允许嵌套 set_nest_tk_list
void set_nest_tk_list();

//不允许嵌套 set_nest_tk_list
void unset_nest_tk_list();

bool is_in_nest();


//===================================其他内核模块使用的软件定时器API=======================
//  均为thread-safety api
//记住： 1. 软件定时器结束的时候执行对应handler 的状态是在M态 权限是最高的 这是其他模块使用需要注意的
//       2. 不能在 handler中调用tick_node_hang 和 tick_node_down 会导致死锁
//       3. 不能在 handler中去获取zombie进程锁  pend进程锁 会死锁！


//  初始化定时器信息节点
void tick_node_init(tick_node_t* tick_node, tick_no_t handler, tick_t tick, tick_hook_t entry_hook, void* entry_param, tick_hook_t exit_hook, void* exit_param);

//  挂软件定时器
void tick_node_hang(tick_node_t* tick_node);

//  摘软件定时器
void tick_node_down(tick_node_t* tick_node);

//========================================================================================


#endif