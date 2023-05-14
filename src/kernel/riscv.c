#include "../include/def.h"
#include "../include/platform.h"
#include "../include/riscv.h"
uint64 inline r_int()
{
    if(r_sstatus()&SIE) return 1;
    return 0;
}
uint64 inline r_tp()
{
    uint64 t;
    asm volatile("add %0,zero,tp":"=r"(t));
    return t;
}
uint64 inline r_sip()
{
    uint64 t;
    asm volatile("csrr %0,sip":"=r"(t));
    return t;
}
uint64 inline r_mip()
{
    uint64 t;
    asm volatile("csrr %0,mip":"=r"(t));
    return t;
}
uint64 inline r_sie()
{
    uint64 t;
    asm volatile("csrr %0,sie":"=r"(t));
    return t;
}
uint64 inline r_mie()
{
    uint64 t;
    asm volatile("csrr %0,mie":"=r"(t));
    return t;
}
uint64 inline r_satp()
{
    uint64 t;
    asm volatile("csrr %0,satp":"=r"(t));
    return t;
}
uint64 inline r_sepc()
{
    uint64 t;
    asm volatile("csrr %0,sepc":"=r"(t));
    return t;
}
uint64 inline r_mepc()
{
    uint64 t;
    asm volatile("csrr %0,mepc":"=r"(t));
    return t;
}
uint64 inline r_stval()
{
    uint64 t;
    asm volatile("csrr %0,stval":"=r"(t));
    return t;
}
uint64 inline r_mtval()
{
    uint64 t;
    asm volatile("csrr %0,mtval":"=r"(t));
    return t;
}
uint64 inline r_stvec()
{
    uint64 t;
    asm volatile("csrr %0,stvec":"=r"(t));
    return t;
}
uint64 inline r_mtvec()
{
    uint64 t;
    asm volatile("csrr %0,mtvec":"=r"(t));
    return t;
}
uint64 inline r_scause()
{
    uint64 t;
    asm volatile("csrr %0,scause":"=r"(t));
    return t;
}
uint64 inline r_mcause()
{
    uint64 t;
    asm volatile("csrr %0,mcause":"=r"(t));
    return t;
}
uint64 inline r_sstatus()
{
    uint64 t;
    asm volatile("csrr %0,sstatus":"=r"(t));
    return t;
}
uint64 inline r_mstatus()
{
    uint64 t;
    asm volatile("csrr %0,mstatus":"=r"(t));
    return t;
}
uint64 inline r_mhartid()
{
    uint64 t;
    asm volatile("csrr %0,mhartid":"=r"(t));
    return t;
}
uint64 inline r_mideleg()
{
    uint64 t;
    asm volatile("csrr %0,mideleg":"=r"(t));
    return t;
}
uint64 inline r_medeleg()
{
    uint64 t;
    asm volatile("csrr %0,medeleg":"=r"(t));
    return t;
}
uint64 inline r_sscratch()
{
    uint64 t;
    asm volatile("csrr %0,sscratch":"=r"(t));
    return t;
}
uint64 inline r_mscratch()
{
    uint64 t;
    asm volatile("csrr %0,mscratch":"=r"(t));
    return t;
}

void inline w_int_on()
{
    w_sstatus(r_sstatus()|SIE_S);
}
void inline w_int_off()
{
    w_sstatus(r_sstatus()&(~SIE_S));
}
void inline w_tp(uint64 x)
{
    asm volatile("add tp,zero,%0"::"r"(x));
}
void inline w_sip(uint64 x)
{
    asm volatile("csrw sip,%0"::"r"(x));
}
void inline w_mip(uint64 x)
{
    asm volatile("csrw mip,%0"::"r"(x));
}
void inline w_sie(uint64 x)
{
    asm volatile("csrw sie,%0"::"r"(x));
}
void inline w_mie(uint64 x)
{
    asm volatile("csrw mie,%0"::"r"(x));
}
void inline w_satp(uint64 x)
{
    asm volatile("csrw satp,%0"::"r"(x));
}
void inline w_sepc(uint64 x)
{
    asm volatile("csrw sepc,%0"::"r"(x));
}
void inline w_mepc(uint64 x)
{
    asm volatile("csrw mepc,%0"::"r"(x));
}
void inline w_stval(uint64 x)
{
    asm volatile("csrw stval,%0"::"r"(x));
}
void inline w_mtval(uint64 x)
{
    asm volatile("csrw mtval,%0"::"r"(x));
}
void inline w_stvec(uint64 x)
{
    asm volatile("csrw stvec,%0"::"r"(x));
}
void inline w_mtvec(uint64 x)
{
    asm volatile("csrw mtvec,%0"::"r"(x));
}
void inline w_scause(uint64 x)
{
    asm volatile("csrw scause,%0"::"r"(x));
}
void inline w_mcause(uint64 x)
{
    asm volatile("csrw mcause,%0"::"r"(x));
}
void inline w_sstatus(uint64 x)
{
    asm volatile("csrw sstatus,%0"::"r"(x));
}
void inline w_mstatus(uint64 x)
{
    asm volatile("csrw mstatus,%0"::"r"(x));
}
// void inline w_mhartid(uint64 x)
// {
//     asm volatile("csrw mhartid,%0"::"r"(x));
// }
void inline w_mideleg(uint64 x)
{
    asm volatile("csrw mideleg,%0"::"r"(x));
}
void inline w_medeleg(uint64 x)
{
    asm volatile("csrw medeleg,%0"::"r"(x));
}
void inline w_sscratch(uint64 x)
{
    asm volatile("csrw sscratch,%0"::"r"(x));
}
void inline w_mscratch(uint64 x)
{
    asm volatile("csrw mscratch,%0"::"r"(x));
}
void inline w_pmpaddr0(uint64 x)
{
    asm volatile("csrw pmpaddr0,%0"::"r"(x));
}
void inline w_pmpcfg0(uint64 x)
{
    asm volatile("csrw pmpcfg0,%0"::"r"(x));
}