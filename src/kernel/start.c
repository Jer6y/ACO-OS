#include "Los_type.h"
#include "Los_err.h"
#include "bsp_port.h"
#include "arch_port.h"
#include "mtime_trap.h"

extern void main();

static void tick_init()
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
    w_mstatus(r_mstatus()|MPP_S|MPIE_S);
    
    //w_mie(r_mie()|MEIE_S|MTIE_S|MSIE_S);
    
    //跳转到main
    w_mepc((uint64)main);

    w_mtvec((uint64)mtime_trap);

    w_mscratch((uint64)m_scratch);
    
    tick_init();
    //产生在M模式下的中断和异常都代理给S模式
    //timer-interruput 是不能被代理的 必须在m模式下处理
    //这里注意m模式是没有分页机制的 是直接使用的物理地址
    w_medeleg(0xffff);
    w_mideleg(0xffff);

    //物理内存保护
    w_pmpaddr0(0x3fffffffffffffull);
    w_pmpcfg0(0xf);
      
    w_sie(r_sie()|SEIE_S|STIE_S|SSIE_S);
    asm volatile("mret");
}