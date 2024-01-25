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
#include "mln_list.h"
#include "tick.h"
//上面是依赖 [依赖哪些公共库和哪些模块]
//下面是实现 [实现了哪些公共库或者哪些模块]
#include "mtime_trap.h"

uint64 m_scratch[CPUS][3];
// 0 - t0
// 1 - sp
// 2 - t1

uint8  m_stack_sp[CPUS][1024]__attribute__((aligned(16)));

uint64 hart_thr_num_trig;

void time_normal_handler()
{
    uint64 tick_next = clint_get_tick();
    uint64 hart = r_mhartid();
    tick_next += CPU_FREQUENCE/(tick_s_to_ms(1)/tick_per_ms());
    clint_set_tcmp(hart,tick_next);
    w_sip(r_sip()|SSIP_S);
}

void mtime_handler()
{
    uint64 tick_next = clint_get_tick();
    uint64 hart = r_mhartid();
    tick_next += CPU_FREQUENCE/(tick_s_to_ms(1)/tick_per_ms());
    clint_set_tcmp(hart,tick_next);
    hart_thr_num_trig++;
    tick_update();
}

// 这里的设计思想理念是:
// 1. 为什么要在list_update 打开中断
//      : 在睡眠的线程数目多的情况下 , list_update 的时间很长, 会影响 tick 的更新
//      : 允许打开中断 允许嵌套
//      : 所以我们核3 的mtick 的中断处理和其他的不一样，是直接压栈的
//      : 允许嵌套中断
// 2. 为什么要提前判断嵌套:
//      : 我们嵌套只想要不延误tick 的更新 以及 trig的更新
//      : 如果tail_handler发现嵌套了，说明在嵌套前就是处于tick更新中
//      : 如果再更新tick list 只会发生 临界区混乱, 并发问题
// 所以核心设计思想是:
// 允许在处理tick_list tick量 以及 trig量 但不允许更新tick list
// 小细节: hart_thr_num_trig 变量是临界区的变量，所以每次监测这边变量的时候都是关掉中断的
// 其次 我们只允许 核3 做中断相关的处理，不允许其他核 update tick list
void mtime_tail_handler()
{
    if(is_in_nest())
    {
        return;
    }  
    set_nest_tk_list();
    while(hart_thr_num_trig > 0)
    {
        w_int_on_m();
        tick_list_update();
        w_int_off_m();
        hart_thr_num_trig--;
    }
    unset_nest_tk_list();
}