#ifndef DEF_H
#define DEF_H

#include "types.h"


struct cpu;
struct spinlock;
struct proc;

//for cpu
struct cpu
{
    //目前正在运行的进程
    //如果为0,就表示当前在调度器线程同时没有进程在运行
    struct proc* run;
    //记录当前不允许中断的层数
    //每获得一把锁 或者调用一次push_on就会加一层
    size_t int_depth;
    
    //记录在不允许中断前的中断使能状态
    uint8  int_enable;
};
struct spinlock
{
    uint8 value;
    char name;
};


//boot.s 开机第一个引导函数
/*
 * 让sp指向每个核的初始化栈的栈顶
 * 将每个核的ID加载到tp-register中
 */
void _start();

//start.c 引导后在M模式下初始化
/*
 * 初始化时钟中断
 * 跳入S模式
 */
void start();

//main.c
/*
 * 初始化内核
 * 进入调度器
 */
void main();


//kernel_vec.s
/*
 * timer-interruput handler
 * 1. 重新设置下一次的阈值
 * 2. 通过sip手动打开S模式的软件中断
 */
void timer_vec();


//uart.c 
//串口驱动程序:

//串口初始化
void uart_init();

//

//riscv.c
/*读riscv-csr的c描述的函数*/
uint64 r_int();
uint64 r_tp();
uint64 r_sip();
uint64 r_mip();
uint64 r_sie();
uint64 r_mie();
uint64 r_satp();
uint64 r_sepc();
uint64 r_mepc();
uint64 r_stval();
uint64 r_mtval();
uint64 r_stvec();
uint64 r_mtvec();
uint64 r_scause();
uint64 r_mcause();
uint64 r_sstatus();
uint64 r_mstatus();
uint64 r_mhartid();
uint64 r_mideleg();
uint64 r_medeleg();
uint64 r_sscratch();
uint64 r_mscratch();


/*写riscv-csr的c描述的函数*/
void w_int_on();
void w_int_off();
void w_tp(uint64 x);
void w_sip(uint64 x);
void w_mip(uint64 x);
void w_sie(uint64 x);
void w_mie(uint64 x);
void w_satp(uint64 x);
void w_sepc(uint64 x);
void w_mepc(uint64 x);
void w_stval(uint64 x);
void w_mtval(uint64 x);
void w_stvec(uint64 x);
void w_mtvec(uint64 x);
void w_scause(uint64 x);
void w_mcause(uint64 x);
void w_sstatus(uint64 x);
void w_mstatus(uint64 x);
// hartid 为只读 是不允许写的!
// void w_mhartid(uint64 x);

void w_mideleg(uint64 x);
void w_medeleg(uint64 x);
void w_sscratch(uint64 x);
void w_mscratch(uint64 x);
void w_pmpaddr0(uint64 x);
void w_pmpcfg0(uint64 x);


#endif