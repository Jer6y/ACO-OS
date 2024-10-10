#ifndef LOS_ERR_H
#define LOS_ERR_H

typedef enum LOS_ERR_E {
    ERR_NONE                 = 0    ,
    ERR_PTR_NULL             =2398u ,
    ERR_MEMORY_EMPTY                ,
    ERR_OBJ_MAGIC_ERR               ,
    ERR_PLIC_DEV_TYPE               ,
    ERR_PLIC_DEV_PRIO               ,
    ERR_TICK_HANG_ERR               ,

    ERR_SEM_INFO_ERR                ,

    ERR_MUTEX_INFO_ERR              ,
    ERR_MUTEX_CRTICAL               ,
    ERR_MUTEX_NONEED_UNLOCK         ,

    ERR_SCHED_LOCKED                ,
    ERR_SCHED_UNLOCKED              ,
    ERR_PROC_INFO_ERR               ,
    
    ERR_TIMER_INFO_ERR              ,
    ERR_TIMER_HANDLING              ,

    ERR_TICK_HANDLEING              ,

    ERR_RISCV_CPU_TYPE              ,
    
    ERR_SIG_OVER_SIZE               ,

    ERR_MMU_MISALIGN                ,
} LOS_ERR_T;


#endif