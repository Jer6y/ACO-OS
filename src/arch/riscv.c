#include "Los_type.h"
#include "platform.h"
//上面是依赖 [依赖哪些公共库和哪些模块]
//下面是实现 [实现了哪些公共库或者哪些模块]
#include "riscv.h"

uint64 hwi_lock()
{
    uint64 x = r_int_s();
    w_int_off_s();
    return x;
}

uint64 hwi_unlock()
{
    uint64 x = r_int_s();
    w_int_on_s();
    return x;
}

void hwi_restore(uint64 int_bef)
{
    if(int_bef & 1)
    {
        hwi_unlock();
    }
    else
    {
        hwi_lock();
    }
}


void  sfence()
{
    asm volatile("sfence.vma zero,zero");
}

uint64  r_int_s()
{
    if(r_sstatus()&SIE) return 1;
    return 0;
}

uint64  r_int_m()
{
    if(r_mstatus()&MIE) return 1;
    return 0;
}

uint64  r_tp()
{
    uint64 t;
    asm volatile("add %0,zero,tp":"=r"(t));
    return t;
}
uint64  r_sip()
{
    uint64 t;
    asm volatile("csrr %0,sip":"=r"(t));
    return t;
}
uint64  r_mip()
{
    uint64 t;
    asm volatile("csrr %0,mip":"=r"(t));
    return t;
}
uint64  r_sie()
{
    uint64 t;
    asm volatile("csrr %0,sie":"=r"(t));
    return t;
}
uint64  r_mie()
{
    uint64 t;
    asm volatile("csrr %0,mie":"=r"(t));
    return t;
}
uint64  r_satp()
{
    uint64 t;
    asm volatile("csrr %0,satp":"=r"(t));
    return t;
}
uint64  r_sepc()
{
    uint64 t;
    asm volatile("csrr %0,sepc":"=r"(t));
    return t;
}
uint64  r_mepc()
{
    uint64 t;
    asm volatile("csrr %0,mepc":"=r"(t));
    return t;
}
uint64  r_stval()
{
    uint64 t;
    asm volatile("csrr %0,stval":"=r"(t));
    return t;
}
uint64  r_mtval()
{
    uint64 t;
    asm volatile("csrr %0,mtval":"=r"(t));
    return t;
}
uint64  r_stvec()
{
    uint64 t;
    asm volatile("csrr %0,stvec":"=r"(t));
    return t;
}
uint64  r_mtvec()
{
    uint64 t;
    asm volatile("csrr %0,mtvec":"=r"(t));
    return t;
}
uint64  r_scause()
{
    uint64 t;
    asm volatile("csrr %0,scause":"=r"(t));
    return t;
}
uint64  r_mcause()
{
    uint64 t;
    asm volatile("csrr %0,mcause":"=r"(t));
    return t;
}
uint64  r_sstatus()
{
    uint64 t;
    asm volatile("csrr %0,sstatus":"=r"(t));
    return t;
}
uint64  r_mstatus()
{
    uint64 t;
    asm volatile("csrr %0,mstatus":"=r"(t));
    return t;
}
uint64  r_mhartid()
{
    uint64 t;
    asm volatile("csrr %0,mhartid":"=r"(t));
    return t;
}
uint64  r_mideleg()
{
    uint64 t;
    asm volatile("csrr %0,mideleg":"=r"(t));
    return t;
}
uint64  r_medeleg()
{
    uint64 t;
    asm volatile("csrr %0,medeleg":"=r"(t));
    return t;
}
uint64  r_sscratch()
{
    uint64 t;
    asm volatile("csrr %0,sscratch":"=r"(t));
    return t;
}
uint64  r_mscratch()
{
    uint64 t;
    asm volatile("csrr %0,mscratch":"=r"(t));
    return t;
}

void  w_int_on_s()
{
    w_sstatus(r_sstatus()|SIE_S);
}

void  w_int_on_m()
{
    w_mstatus(r_sstatus()|MIE_S);
}

void  w_int_off_s()
{
    w_sstatus(r_sstatus()&(~SIE_S));
}

void  w_int_off_m()
{
    w_mstatus(r_mstatus()&(~MIE_S));
}

void  w_tp(uint64 x)
{
    asm volatile("add tp,zero,%0"::"r"(x));
}
void  w_sip(uint64 x)
{
    asm volatile("csrw sip,%0"::"r"(x));
}
void  w_mip(uint64 x)
{
    asm volatile("csrw mip,%0"::"r"(x));
}
void  w_sie(uint64 x)
{
    asm volatile("csrw sie,%0"::"r"(x));
}
void  w_mie(uint64 x)
{
    asm volatile("csrw mie,%0"::"r"(x));
}
void  w_satp(uint64 x)
{
    asm volatile("csrw satp,%0"::"r"(x));
}
void  w_sepc(uint64 x)
{
    asm volatile("csrw sepc,%0"::"r"(x));
}
void  w_mepc(uint64 x)
{
    asm volatile("csrw mepc,%0"::"r"(x));
}
void  w_stval(uint64 x)
{
    asm volatile("csrw stval,%0"::"r"(x));
}
void  w_mtval(uint64 x)
{
    asm volatile("csrw mtval,%0"::"r"(x));
}
void  w_stvec(uint64 x)
{
    asm volatile("csrw stvec,%0"::"r"(x));
}
void  w_mtvec(uint64 x)
{
    asm volatile("csrw mtvec,%0"::"r"(x));
}
void  w_scause(uint64 x)
{
    asm volatile("csrw scause,%0"::"r"(x));
}
void  w_mcause(uint64 x)
{
    asm volatile("csrw mcause,%0"::"r"(x));
}
void  w_sstatus(uint64 x)
{
    asm volatile("csrw sstatus,%0"::"r"(x));
}
void  w_mstatus(uint64 x)
{
    asm volatile("csrw mstatus,%0"::"r"(x));
}
// void  w_mhartid(uint64 x)
// {
//     asm volatile("csrw mhartid,%0"::"r"(x));
// }
void  w_mideleg(uint64 x)
{
    asm volatile("csrw mideleg,%0"::"r"(x));
}
void  w_medeleg(uint64 x)
{
    asm volatile("csrw medeleg,%0"::"r"(x));
}
void  w_sscratch(uint64 x)
{
    asm volatile("csrw sscratch,%0"::"r"(x));
}
void  w_mscratch(uint64 x)
{
    asm volatile("csrw mscratch,%0"::"r"(x));
}
void  w_pmpaddr0(uint64 x)
{
    asm volatile("csrw pmpaddr0,%0"::"r"(x));
}
void  w_pmpcfg0(uint64 x)
{
    asm volatile("csrw pmpcfg0,%0"::"r"(x));
}