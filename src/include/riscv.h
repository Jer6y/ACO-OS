#ifndef RISCV_H
#define RISCV_H
/*
 * riscv.h 定义了与rv64-csr 相关的位
 */

//S-mode:
//sstatus:
//S-mode 中断使能总开关
//SIE_S  是置1SIE的数
//SIE_R  是重置SIE的数
#define SIE  (1ull<<1)
    #define SIE_S (1ull<<1)
    #define SIE_R (0ull<<1)

//S-mode env-ret跳转到的模式
#define SPP  (1ull<<8)
    #define SPP_U (0ull<<8)
    #define SPP_S (1ull<<8)

//S-mode 若SUM置1 则S模式可以访问U模式的page 反之
#define SUM  (1ull<<18)
    #define SUM_S (1ull<<18)
    #define SUM_R (0ull<<18)

//S-mode env-ret 跳转时SPIE会覆盖SIE
#define SPIE (1ull<<5)
    #define SPIE_S (1ull<<5)
    #define SPIE_R (0ull<<5)

#define SEIE (1u<<9)
    #define SEIE_S (1u<<9)
    #define SEIE_R (0u<<9)

#define STIE (1u<<5)
    #define STIE_S (1u<<5)
    #define STIE_R (0u<<5)

#define SSIE (1u<<1)
    #define SSIE_S (1u<<1)
    #define SSIE_R (0u<<1)
//M-mode:
#define MIE  (1ull<<3)
    #define MIE_S (1ull<<3)
    #define MIE_R (0ull<<3)

#define MPP  (3ull<<11)
    #define MPP_U (0ull<<11)
    #define MPP_S (1ull<<11)

#define MPIE (1ull<<7)
    #define MPIE_S (1ull<<7)
    #define MPIE_R (0ull<<7)

#define MEIE (1u<<11)
    #define MEIE_S (1u<<11)
    #define MEIE_R (0u<<11)

#define MTIE (1u<<7)
    #define MTIE_S (1u<<7)
    #define MTIE_R (0u<<7)

#define MSIE (1u<<3)
    #define MSIE_S (1u<<3)
    #define MSIE_R (0u<<3)

#endif