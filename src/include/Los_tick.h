/*
 * port to user
 * tick 模块的API
 *
 */
#ifndef LOS_TICK_H
#define LOS_TICK_H

typedef uint64 tick_t;

#define tick_s_to_ms(x)  ((x)*1000)

#define tick_ms_to_us(x) ((x)*1000)

#define tick_us_to_ms(x) ((x)/1000)

#define tick_us_to_s(x)  ((x)/1000000)

//外部模块获取每个tick的ms数，方便对接硬件模块进行管理
#define tick_per_ms() 100

// 获取当前系统开机了多少个tick
tick_t tick_get();

// 获取对应tick到ms的转化
uint64 tick_to_ms(tick_t tick);

// 获取对应tick到us的转化
uint64 tick_to_us(tick_t tick);


#endif