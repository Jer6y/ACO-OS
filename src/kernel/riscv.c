#include "../include/def.h"
#include "../include/platform.h"
uint64 r_int()
{
    if(r_sstatus()&SIE) return 1;
    return 0;
}
uint64 r_tp()
{
    uint64 t;
    asm volatile("add %0,zero,tp":"=r"(t));
    return t;
}
uint64 r_sip()
{
    uint64 t;
    asm volatile("csrr %0,sip":"=r"(t));
    return t;
}
uint64 r_mip()
{
    uint64 t;
    asm volatile("csrr %0,mip":"=r"(t));
    return t;
}
uint64 r_sie()
{
    uint64 t;
    asm volatile("csrr %0,sie":"=r"(t));
    return t;
}
uint64 r_mie()
{
    uint64 t;
    asm volatile("csrr %0,mie":"=r"(t));
    return t;
}
uint64 r_satp()
{
    uint64 t;
    asm volatile("csrr %0,satp":"=r"(t));
    return t;
}
uint64 r_sepc()
{
    uint64 t;
    asm volatile("csrr %0,sepc":"=r"(t));
    return t;
}
uint64 r_mepc()
{
    uint64 t;
    asm volatile("csrr %0,mepc":"=r"(t));
    return t;
}
uint64 r_stval()
{
    uint64 t;
    asm volatile("csrr %0,stval":"=r"(t));
    return t;
}
uint64 r_mtval()
{
    uint64 t;
    asm volatile("csrr %0,mtval":"=r"(t));
    return t;
}
uint64 r_stvec()
{
    uint64 t;
    asm volatile("csrr %0,stvec":"=r"(t));
    return t;
}
uint64 r_mtvec()
{
    uint64 t;
    asm volatile("csrr %0,mtvec":"=r"(t));
    return t;
}
uint64 r_scause()
{
    uint64 t;
    asm volatile("csrr %0,scause":"=r"(t));
    return t;
}
uint64 r_mcause()
{
    uint64 t;
    asm volatile("csrr %0,mcause":"=r"(t));
    return t;
}
uint64 r_sstatus()
{
    uint64 t;
    asm volatile("csrr %0,sstatus":"=r"(t));
    return t;
}
uint64 r_mstatus()
{
    uint64 t;
    asm volatile("csrr %0,mstatus":"=r"(t));
    return t;
}
uint64 r_mhartid()
{
    uint64 t;
    asm volatile("csrr %0,mhartid":"=r"(t));
    return t;
}
uint64 r_mideleg()
{
    uint64 t;
    asm volatile("csrr %0,mideleg":"=r"(t));
    return t;
}
uint64 r_medeleg()
{
    uint64 t;
    asm volatile("csrr %0,medeleg":"=r"(t));
    return t;
}
uint64 r_sscratch()
{
    uint64 t;
    asm volatile("csrr %0,sscratch":"=r"(t));
    return t;
}
uint64 r_mscratch()
{
    uint64 t;
    asm volatile("csrr %0,mscratch":"=r"(t));
    return t;
}