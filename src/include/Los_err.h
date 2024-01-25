#ifndef LOS_ERR_H
#define LOS_ERR_H

typedef enum LOS_ERR_E {
    ERR_NONE                 = 0    ,
    ERR_PTR_NULL                    ,
    ERR_PLIC_DEV_TYPE        =2398u ,
    ERR_PLIC_DEV_PRIO               ,
    ERR_TICK_HANG_ERR               ,

    ERR_SCHED_LOCKED                ,
    ERR_SCHED_UNLOCKED              ,
    ERR_PROC_INFO_ERR               ,

    ERR_TICK_HANDLEING              ,

    ERR_RISCV_CPU_TYPE              ,
    
    ERR_SIG_OVER_SIZE               ,

    ERR_MMU_MISALIGN                ,
} LOS_ERR_T;


#endif