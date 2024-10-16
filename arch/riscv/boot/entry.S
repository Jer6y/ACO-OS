#include <aco/assembly.h>
#include <aco/linkage.h>
#include <asm/fix_address.h>

ASM_EXTERN(__sinitstack_section)
ASM_EXTERN(main)

HEADFUNC_START(_start)
    mv   tp, a1
    mv   s0, a0
#ifdef CONFIG_BOOT_SMP
    bneq tp, zero, 1f
#endif
    la   t0, __sinitstack_section
    li 	 t1, RISCV_INIT_STKSIZE
    add  sp, t0, t1
    mv   a0, s0
    mv   a1, s1
    call main
1:
    /* TODO Support for SMP BOOT*/
    wfi
    j 1b
HEADFUNC_END(_start)
