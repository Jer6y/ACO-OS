#ifndef DEF_H
#define DEF_H

#include "types.h"
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

//riscv.c
/*读riscv-csr的c描述的函数*/
uint64 r_mip();
uint64 r_mie();
uint64 r_mepc();
uint64 r_mtvec();
uint64 r_mcause();
uint64 r_mstatus();
uint64 r_mhartid();

/*写riscv-csr的c描述的函数*/

#endif