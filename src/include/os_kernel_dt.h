#ifndef OS_KERNEL_DT_H
#define OS_KERNEL_DT_H
#include <os.h>

/*****************************************************内核数据区************************************************
*************************************************************************************************************/

/**************进程相关****************/

//内核页表
pagetable* kernel_pgt;

/**************物理内存分配器***********/

struct memory_unit kmem;

uint8  bitmap[TOTALPAGES];

/**************进程相关****************/

//进出临界区的判断变量
int             int_enable      ;
int             depth_int       ; 
//目前RUNNING的程序
struct proc*    run_proc        ;
//0级就绪队列
struct proc*    ready_start_z   ;
struct proc*    ready_end_z     ;
//1级就绪队列
struct proc*    ready_start_o   ;
struct proc*    ready_end_o     ;
//2级就绪队列
struct proc*    ready_start_t   ;
struct proc*    ready_end_t     ;
//阻塞队列
struct proc*    block_start     ;
struct proc*    block_end       ;
//僵尸队列
struct proc*    zombie_start    ;
struct proc*    zombie_end      ;

/**************************************/

/**************时钟相关****************/

//开机到现在经过的时钟tick数目
uint64 tick_passed;

//每个tick的ms数目
uint64 tick_per_ms;



/************************************/

/*************串口输出缓冲区***********/

#define OUT_BUF_SIZE 128
char output_buffer[OUT_BUF_SIZE];
uint32 output_w;
uint32 output_r;

/************************************/

/*************串口输入缓冲区***********/

#define INPUT_BUF_SIZE 1024

char input_buf[INPUT_BUF_SIZE];
uint32 input_r;
uint32 input_w;
uint32 input_e;

/************************************/

#endif