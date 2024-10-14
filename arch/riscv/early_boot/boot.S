
#include <aco/assembly.h>
#include <asm/link.h>

ASM_EXTERN(main)

HEADFUNC_START(_start)
    la   t0,__sinitstack_section
    li   t1,RISCV_INIT_STKSIZE
    add  t0,t0,t1
    add  sp,t0,zero
    call main
__spin:
    j __spin
HEADFUNC_END(_start)
