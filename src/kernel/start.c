#include <os.h>
#include <riscv.h>
char ks_stack[CPUS*PAGESIZE]__attribute__((aligned(16)));
uint64 timer_mscratch[CPUS][5];




#define CLINT_TIME             (CLINT_START+0xBFF8)
#define CLINT_TIMECMP(hart_id) (CLINT_START+0x4000+8*(hart_id))

static void timer_init()
{
    uint64 cpu_id = r_mhartid();

    //设置间隔位8000000个周期
    //qemu 中一个周期是1ps 约等于8ms
    uint64 intervel = 8000000;

    uint64 x = *(uint64 *)CLINT_TIME;
    x+=intervel;
    *(uint64 *)CLINT_TIMECMP(cpu_id)=x;

    timer_mscratch[cpu_id][3] =(uint64)CLINT_TIMECMP(cpu_id);
    timer_mscratch[cpu_id][4] =intervel;
    w_mscratch((uint64)&timer_mscratch[cpu_id][0]);
    
    w_mie(r_mie()|MTIE_S);
    w_mtvec((uint64)timer_vec);
}

void start()
{
    //关闭S-mode下的分页机制
    w_satp(0);

    //修改mstatus的值
    //1. 进行mret后的模式
    //2. 进行mret后MIE的状态
    w_mstatus(r_mstatus()|MPP_S|MPIE_S);
    
    //跳转到main
    w_mepc((uint64)main);

    //产生在M模式下的中断和异常都代理给S模式
    //timer-interruput 是不能被代理的 必须在m模式下处理
    //这里注意m模式是没有分页机制的 是直接使用的物理地址
    w_medeleg(0xffff);
    w_mideleg(0xffff);

    //物理内存保护
    w_pmpaddr0(0x3fffffffffffffull);
    w_pmpcfg0(0xf);

    //初始化timer-interrupt 
    timer_init();
    
    w_sie(r_sie()|SEIE_S|STIE_S|SSIE_S);
    asm volatile("mret");
}