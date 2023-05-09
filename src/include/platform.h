#ifndef PLATFORM_H
#define PLATFORM_H

//CPU核心数
#define CPUS 1

//内核基地址 闭区间
#define KERNEL_BASE (0x80000000ul)
 
//页粒度 4k
#define PAGESIZE (4096)

//物理地址结束的地方 闭区间
#define RAM_END  ((KERNEL_BASE+128*1024*1024)-1)

#define SIE (1<<1)

#endif