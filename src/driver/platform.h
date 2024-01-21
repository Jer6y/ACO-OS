/*
 * 这个文件存放一些板载平台信息 [就是qemu-virt 模拟出来的这款板子]
 *    CPU核心数 ,  
 *    UART起始地址 ,
 *    页大小 , 
 *    PLIC 起始地址等
 */
#ifndef PLATFORM_H
#define PLATFORM_H

//CPU核心数
#define CPUS            
//虚拟地址最高处                        闭区间
#define MAX_VA          ((1ull<<39)-1)
//内核基地址                            闭区间
#define KERNEL_BASE     (0x80000000ull) 
//页粒度                                4k
#define PAGESIZE        (4096)
//物理地址结束的地方                    闭区间
#define RAM_END         ((KERNEL_BASE+128*1024*1024)-1)

//串口                              
#define UART0                       0x10000000ull
#define UART0_IRQ                   10

// virtio mmio interface
#define VIRTIO0                     0x10001000ull
#define VIRTIO0_IRQ                 1

//本地中断管理设备 时钟中断 + 软件中断                              
#define CLINT                       0x2000000ull

//外部中断管理设备
#define PLIC                        0x0c000000L

//TRAPFRAM地址

#define TRAPFRAME       0x3ffffff000ull 
//总页数
#define TOTALPAGES      ((128*1024*1024)/4096)
#endif