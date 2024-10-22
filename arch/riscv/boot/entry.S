#include <aco/assembly.h>
#include <aco/linkage.h>

ASM_EXTERN(__sinitstack_section)
ASM_EXTERN(setup_earlymm)
ASM_EXTERN(setup_earlyvm)
ASM_EXTERN(main)

HEADFUNC_START_ALIGN(_start,4)
    mv   tp, a0
    mv   s0, a1
#ifdef CONFIG_BOOT_SMP
    bneq tp, zero, 1f
#endif
    la   t0, __sinitstack_section
    li 	 t1, RISCV_INIT_STKSIZE
    add  sp, t0, t1
    call setup_earlymm
    mv   a0, s0
    mv   a1, tp
    call setup_earlyvm
    call setup_satp
    call main
1:
    /* TODO Support for SMP BOOT*/
    wfi
    j 1b
HEADFUNC_END(_start)

HEADFUNC_START_ALIGN(setup_satp,4)
    li   t0, RISCV_VA2PA_OFFSET
    add  ra, t0, ra  //fix ra address
    add  sp, t0, sp  //fix sp address
    la   t1, 2f
    add  t1, t0, t1  //fix next address
    csrw stvec, t1
    csrw satp, a0
    sfence.vma zero, zero
1:  
    j 1b
ASM_ALIGN(4)
2:
    ret
HEADFUNC_END(setup_satp)
