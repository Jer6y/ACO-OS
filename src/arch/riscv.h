/*
 * riscv 架构公共库
 * 放一些与架构相关的函数
 * 架构相关的内容
 */
#ifndef RISCV_H
#define RISCV_H

#define CPU_MODE_M  1
#define CPU_MODE_S  2
#define CPU_MODE_H  4
#define CPU_MODE_U  8

#define SSIP  (1ull<<1)
#define SSIP_S SSIP
#define SSIP_R (~SSIP)

#define SIE   (1ull<<1)
#define SIE_S (1ull<<1)
#define SIE_R (0ull<<1)

//S-mode env-ret跳转到的模式
#define SPP   (1ull<<8)
#define SPP_U (0ull<<8)
#define SPP_S (1ull<<8)

//S-mode 若SUM置1 则S模式可以访问U模式的page 反之
#define SUM   (1ull<<18)
#define SUM_S (1ull<<18)
#define SUM_R (0ull<<18)

//S-mode env-ret 跳转时SPIE会覆盖SIE
#define SPIE  (1ull<<5)
#define SPIE_S (1ull<<5)
#define SPIE_R (0ull<<5)

#define SEIE   (1u<<9)
#define SEIE_S (1u<<9)
#define SEIE_R (0u<<9)

#define STIE   (1u<<5)
#define STIE_S (1u<<5)
#define STIE_R (0u<<5)

#define SSIE   (1u<<1)
#define SSIE_S (1u<<1)
#define SSIE_R (0u<<1)
//M-mode:
#define MIE   (1ull<<3)
#define MIE_S (1ull<<3)
#define MIE_R (0ull<<3)

#define MPP   (3ull<<11)
#define MPP_U (0ull<<11)
#define MPP_S (1ull<<11)
#define MPP_M (3ull<<11)

#define MPIE   (1ull<<7)
#define MPIE_S (1ull<<7)
#define MPIE_R (0ull<<7)

#define MEIE   (1u<<11)
#define MEIE_S (1u<<11)
#define MEIE_R (0u<<11)

#define MTIE   (1u<<7)
#define MTIE_S (1u<<7)
#define MTIE_R (0u<<7)

#define MSIE   (1u<<3)
#define MSIE_S (1u<<3)
#define MSIE_R (0u<<3)

void  sfence();
//flush the satp table

uint64  r_int_s();

uint64  r_int_m();

uint64  r_tp();

uint64  r_sip();

uint64  r_mip();

uint64  r_sie();

uint64  r_mie();

uint64  r_satp();

uint64  r_sepc();

uint64  r_mepc();

uint64  r_stval();

uint64  r_mtval();

uint64  r_stvec();

uint64  r_mtvec();

uint64  r_scause();

uint64  r_mcause();

uint64  r_sstatus();

uint64  r_mstatus();

uint64  r_mhartid();

uint64  r_mideleg();

uint64  r_medeleg();

uint64  r_sscratch();

uint64  r_mscratch();

void  w_int_on_s();

void  w_int_on_m();

void  w_int_off_s();

void  w_int_off_m();

void  w_tp(uint64 x);

void  w_sip(uint64 x);

void  w_mip(uint64 x);

void  w_sie(uint64 x);

void  w_mie(uint64 x);

void  w_satp(uint64 x);

void  w_sepc(uint64 x);

void  w_mepc(uint64 x);

void  w_stval(uint64 x);

void  w_mtval(uint64 x);

void  w_stvec(uint64 x);

void  w_mtvec(uint64 x);

void  w_scause(uint64 x);

void  w_mcause(uint64 x);

void  w_sstatus(uint64 x);

void  w_mstatus(uint64 x);

void  w_mideleg(uint64 x);

void  w_medeleg(uint64 x);

void  w_sscratch(uint64 x);

void  w_mscratch(uint64 x);

void  w_pmpaddr0(uint64 x);

void  w_pmpcfg0(uint64 x);

uint64  hwi_lock();

uint64  hwi_unlock();

void  hwi_restore(uint64 int_bef);

typedef struct riscv_contex_s {
    //普通寄存器
    uint64 sepc;
    uint64 sstatus;
    uint64 t6;
    uint64 t5;
    uint64 t4;
    uint64 t3;
    uint64 s11;
    uint64 s10;
    uint64 s9;
    uint64 s8;
    uint64 s7;
    uint64 s6;
    uint64 s5;
    uint64 s4;
    uint64 s3;
    uint64 s2;
    uint64 a7;
    uint64 a6;
    uint64 a5;
    uint64 a4;
    uint64 a3;
    uint64 a2;
    uint64 a1;
    uint64 a0;
    uint64 s1;
    uint64 s0;
    uint64 t2;
    uint64 t1;
    uint64 t0;
    uint64 tp;
    uint64 gp;
    uint64 ra;
} riscv_contex_t;


#endif