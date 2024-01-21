#ifndef PLATFORM_H
#define PLATFORM_H

//CPU核心数
#define CPUS            1  
//虚拟地址最高处                        闭区间
#define MAX_VA          ((1ull<<39)-1)
//内核基地址                            闭区间
#define KERNEL_BASE     (0x80000000ull) 
//页粒度                                4k
#define PAGESIZE        (4096)
//物理地址结束的地方                    闭区间
#define RAM_END         ((KERNEL_BASE+128*1024*1024)-1)
//串口                                 IO地址
#define UART_START      0x10000000ull
//时钟                                 IO地址
#define CLINT_START     0x2000000ull
//中断管理设备                          IO地址
#define PLIC_START      0xc000000ull
//TRAPFRAM地址
#define TRAPFRAME       0x3ffffff000ull 
//总页数
#define TOTALPAGES      ((128*1024*1024)/4096)
#endif