#ifndef DEF_H
#define DEF_H

#include "types.h"

#define PAGEUP(pgad)   (((uint64)(pgad)+PAGESIZE)&(~((uint64)PAGESIZE-1)))
#define PAGEDOWN(pgad) ((uint64)(pgad)&(~((uint64)PAGESIZE-1)))
typedef unsigned long long pagetable;
typedef unsigned long long pte_t;

typedef unsigned long long va;

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
    int int_depth;
    
    //记录在不允许中断前的中断使能状态
    uint8  int_enable;
};

struct spinlock
{
    uint8 value;
    char* name;
    struct cpu* cpu;
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
void kernel_vec();

//uart.c 
//串口驱动程序:

//串口初始化
void uart_init();
//中断处理
void uart_int_handler();
//读取单个字符
int uart_getchar();
//同步输送字符
void uart_putc_sync(char c);


// proc.c
// 获得当前正在运行的CPU
// 必须关闭中断
struct cpu* mycpu();


//spinlock.c
void init_lock(struct spinlock* lk,char *name);
void lock(struct spinlock* lk);
void unlock(struct spinlock* lk);
void push_on();
void pop_off();


//kalloc.c
//向物理内存分配器要1页的内存
void* kalloc();
//释放某一页的内存
void  kfree(uint64);
//释放从[start,end]的所有页的内存
void  kfree_arrange(uint64 start,uint64 end);
//初始化物理内存分配器
void  kinit();


// plic.c
// 初始化所需要的IO设备的优先级
// 0默认关闭
void plic_init();
// 每个hart初始化hart的中断使能 以及hart的阈值
void plic_init_hart();
// 发生外部中断时 hart 从自己所属的端口取得中断ID
uint32 plic_claim();
// 处理完成中断后 向端口写处理完的中断ID
void plic_complete(uint32 IID);

//trap.c
//S-mode下trap处理的地方
void kernel_trap();
//S-mode下trap的入口 保存上下文
void trap_init();

//string.c
void *memset(const void *des, uint8 c,size_t n);
void *memchr(const void *str, int c, size_t n);
void *memcpy(void *des, const void *src, size_t n);
int memcmp(const void *str1, const void *str2, size_t n);
char *strrchr(const char *str, int c);
char *strchr(const char *str, int c);
char *strcat(char *dest, const char *src);
int strcmp(const char* src,const char *des);
char *strncpy(char *dest, const char *src, size_t n);
char *strcpy(char *dest, const char *src);
size_t  strlen(const char * src);
// 以下的buf或者str都不应该超过256个字符 否则引发panic
int printf(const char* str,...);
int sprintf(char *buf, const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va* args);
void panic(char *str);
void assert_fail(char* file,int line);
#define assert(x) if(!(x)) { assert_fail(__FILE__,__LINE__); }
//riscv.c
/*读riscv-csr的c描述的函数*/

//vm.c
//将虚拟地址v_ad 到物理地址p_ad 
void kvm_init();
void kvm_flush();
int mmap(pagetable* table,uint64 v_ad,uint64 p_ad,uint64 p_end,int perm);
void kvm_map(pagetable* table,uint64 v_ad,uint64 p_ad,uint64 p_end,int perm);
pte_t* walk(pagetable* table,uint64 v_ad,int is_aloc);


void sfence();
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