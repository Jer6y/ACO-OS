#include "Los_type.h"
#include "Los_err.h"
#include "platform.h"
#include "stdarg.h"
#include "mln_list.h"
#include "string.h"
#include "uart.h"
#include "riscv.h"
#include "plic.h"
#include "clint.h"
#include "spinlock.h"
#include "assert.h"
#include "mtime_trap.h"
#include "tick.h"

extern void main();
extern void entry_hathre();

static void tick_init_start()
{
    uint64 tick_next = clint_get_tick();
    uint64 hart = r_mhartid();
    tick_next += (CPU_FREQUENCE/10); // tick per 0.1s
    clint_set_tcmp(hart,tick_next);
}

char ks_stack[CPUS*PAGESIZE]__attribute__((aligned(16)));
void start()
{
    //关闭S-mode下的分页机制
    w_satp(0);

    //修改mstatus的值
    //1. 进行mret后的模式
    //2. 进行mret后MIE的状态
    if(r_mhartid() == 3) // 我们使用3号核专门做时间相关的处理
    {
        tick_init();     // 初始化一下tick 模块
        w_mstatus(r_mstatus()|MPP_M|MPIE_S);
        w_mie(r_mie()|MTIE_S);
        w_mtvec((uint64)(mtime_trap));
        w_mscratch((uint64)(&(m_scratch[r_mhartid()][0]))); //这里没啥意义
        w_mepc((uint64)entry_hathre);                       //核心3 暂时只处理时钟相关的内容, 没事的时候睡觉去,虽然有点浪费资源,但是目前暂时只做相关的处理
    }
    else
    {
        w_mstatus(r_mstatus()|MPP_S|MPIE_S);
        w_mie(r_mie()|MEIE_S|MSIE_S|MTIE_S);
        w_mtvec((uint64)(time_trap_normal));
        w_mscratch((uint64)(&(m_scratch[r_mhartid()][0])));
        //跳转到main
        w_mepc((uint64)main);
        //产生在M模式下的中断和异常都代理给S模式
        //这里注意m模式是没有分页机制的 是直接使用的物理地址
        w_medeleg(0xffff);
        w_mideleg(0xffff);  
        w_sie(r_sie()|SEIE_S|SSIE_S);
    }
    tick_init_start();
    //物理内存保护
    w_pmpaddr0(0x3fffffffffffffull);
    w_pmpcfg0(0xf);
    asm volatile("mret");
}