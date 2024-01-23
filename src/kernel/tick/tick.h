/*
 * 这个是os tick 模块软件部分 
 * 依赖于具体硬件实现
 * 主要作用: 维护内部 tick量 和 睡眠队列
 */
#ifndef TICK_H
#define TICK_H

typedef struct tick_list_s  tick_list_t;

typedef struct tick_node_s  tick_node_t;

typedef uint64 tick_t;

typedef int (*tick_no_t)(tick_node_t* tick_node);

struct tick_list_s {
    los_type_t obj_type;
    uint32     num_slping;
    spinlock_t tick_lock;
    mln_list_t _list;    
} ;

struct tick_node_s {
    los_type_t obj_type;
    tick_t     sleep_tick;      //目前还剩多少个tick 被唤醒
    tick_t     init_tick;       //初始挂上的时候的tick数目 , 在time_out_handler 返回-1时 会重新加载挂上
                                //可以使用 -1 特性去构造软件定时器
    tick_no_t  time_out_handler;
    mln_list_t _node;
} ;

#define tick_s_to_ms(x)  ((x)*1000)

#define tick_ms_to_us(x) ((x)*1000)

#define tick_us_to_ms(x) ((x)/1000)

#define tick_us_to_s(x)  ((x)/1000000)

// 外部模块获取每个tick的ms数，方便对接硬件模块进行管理
#define tick_per_ms() 100

// tick 模块初始化 只能在OS开启前调用
void tick_init(void);

// 获取当前系统开机了多少个tick
tick_t tick_get();

// 获取对应tick到ms的转化
uint64 tick_to_ms(tick_t tick);

// 获取对应tick到us的转化
uint64 tick_to_us(tick_t tick);

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

void tick_node_init(tick_node_t* tick_node, tick_no_t handler, tick_t tick);

void tick_node_hang(tick_node_t* tick_node);

void tick_node_down(tick_node_t* tick_node);

#endif