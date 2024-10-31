#ifndef __ACO_CPU_H
#define __ACO_CPU_H
//give the api/macro to access cpu information

//porting implementation
#include <asm/cpu.h>

//这是一个非常失败的设计，本来的设计应该是所有硬件信息从 device tree 中拿
//但是没有办法，critical zone api 需要知道 cpu 个数后为每个 cpu 分配一个保存 nest次数的int
//如果要从 dtb 中拿到CPU数量再来分配每个cpu的int的话就是动态的，那么意味着就要依赖 mem子系统
//但是 critical zone api 是 spinlock/rwlock 的依赖，同时 mem子系统会依赖 spinlock
//形成了循环依赖，只有采用这种不优雅的方法来做了
#define  CPUS	((int)(__arch_cpu_num))

/*
 *  Desc : get current cpu id
 *  
 *  Need arch Implement : int __arch_cpu_get(void);
 */
static inline int cpu_getid()
{
	return __arch_cpu_get();
}

/*
 *  Desc : close cpu interrupt
 *  
 *  Need arch Implement : void __arch_cpu_intr_off(void);
 */
static inline void cpu_intr_off()
{
	__arch_cpu_intr_off();
}

/*
 *  Desc : open cpu interrupt
 *
 *  Need arch Implement : void __arch_cpu_intr_on(void);
 */
static inline void cpu_intr_on()
{
	__arch_cpu_intr_on();
}

/*
 *  Desc : get the status of interrupt enabled
 *
 *  Need arch Implement : int __arch_cpu_intr_get(void);
 */
static inline int  cpu_intr_get()
{
	return __arch_cpu_intr_get();
}

/*
 *  Desc : restore cpu interrupt
 *  Pram :
 *		intr_enable : if interrupt enable
 */
static inline void cpu_intr_restore(int intr_enable)
{
	if(intr_enable)
		cpu_intr_on();
	else
		cpu_intr_off();
}



#endif /* __ACO_CPU_H */
