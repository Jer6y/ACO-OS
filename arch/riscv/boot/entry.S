#include <aco/assembly.h>
#include <aco/linkage.h>

ASM_EXTERN(__sinitstack_section)
ASM_EXTERN(setup_vm)

HEADFUNC_START(_start)
    mv   tp, a0
    mv   s0, a1
#ifdef CONFIG_BOOT_SMP
    bneq tp, zero, 1f
#endif
    la   t0, __sinitstack_section
    li 	 t1, RISCV_INIT_STKSIZE
    add  sp, t0, t1
    call setup_mm
    mv   a0, s0
    mv   a1, tp
    call setup_vm
1:
    /* TODO Support for SMP BOOT*/
    wfi
    j 1b
HEADFUNC_END(_start)
