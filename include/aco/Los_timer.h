/*
 * port to user
 * 软件定时器模块API
 *
 */
#ifndef LOS_TIMER_H
#define LOS_TIMER_H

#define TIMER_ONCE      (1u<<0)
#define TIMER_ALWAYS    (1u<<1)
// ONCE 优先级比ALWAYS高 
// 所以同时注册为ONCE 和ALWAYS时 结果为ONCE

#include "Los_tick.h"

typedef uint8 timer_attr_t;


typedef void (*timer_handler_t)(void* param);

typedef uint32 timer_t;
 
typedef struct timer_info_s {
    timer_handler_t     handler;
    timer_attr_t        attr;
    void*               param;
    tick_t              tick;
} timer_info_t;

//注册软件定时器 如果属性为ONCE 那么结束后自动释放
LOS_ERR_T timer_register(timer_t* timer_id, const timer_info_t* timer_info);

//如果注册的软件定时器属性为ALWAYS 需要手动释放
//当然 如果你在软件定时器未到之前就调用这个函数
//那么会提前释放
LOS_ERR_T timer_unregister(timer_t timer_id);

#endif