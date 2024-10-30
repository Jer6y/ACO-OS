#include <aco/assembly.h>
#include <aco/linkage.h>
#include <generated/autoconf.h>

ASM_EXTERN(memset)
ASM_EXTERN(memmove)
ASM_EXTERN(cpu_boot)
ASM_EXTERN(sys_stack)
ASM_EXTERN(boot_harts)
ASM_EXTERN(start_kernel)
ASM_EXTERN(setup_earlyvm)
ASM_EXTERN(_start_kernel)
ASM_EXTERN(__sbss_section)
ASM_EXTERN(__ebss_section)
ASM_EXTERN(kernel_map_init)

HEADFUNC_START_ALIGN(_start,4)
    mv   tp, a0
    mv   s0, a1
#ifdef CONFIG_BOOT_SMP
    bneq tp, zero, 1f
#endif
    la   t0, sys_stack
    li 	 t1, CONFIG_RISCV_INIT_STACKSIZE
    addi t2, tp, 1
    mul  t1, t1, t2
    add  sp, t0, t1
    la   t0, cpu_boot
    sd   tp, 0(t0)
    la   a0, __sbss_section
    la   a1, __ebss_section
    sub  a2, a1, a0
    add  a1, zero, zero
    call memset
    la   a0, dtb
    mv   a1, s0
    li   a2, RISCV_DTB_SIZE
    call memmove
    la   s0, dtb
    mv   a0, s0
    call kernel_map_init
    mv   a0, s0
    mv   a1, tp
    call setup_earlyvm
    la   a0, 2f
    call boot_harts
    call setup_satp
    /* come to a new environment */
    // clear no need registers first
    // s0,tp,sp not clear
    // s0 - dtb address in new environment
    // tp - core id
    // sp - init stack sp in new environment
    add  gp, zero, zero
    add  ra, zero, zero
    add  t0, zero, zero
    add  t1, zero, zero
    add  s1, zero, zero
    add  a0, zero, zero
    add  a1, zero, zero
    add  a2, zero, zero
    add  a3, zero, zero
    add  a4, zero, zero
    add  a5, zero, zero
    add  a6, zero, zero
    add  a7, zero, zero
    add  s2, zero, zero
    add  s3, zero, zero
    add  s4, zero, zero
    add  s5, zero, zero
    add  s6, zero, zero
    add  s7, zero, zero
    add  s8, zero, zero
    add  s9, zero, zero
    add  s10, zero, zero
    add  s11, zero, zero
    add  t3, zero, zero
    add  t4, zero, zero
    add  t5, zero, zero
    add  t6, zero, zero
    
    // set csrs
    li      t0, ((1L << 1) | (1L << 0) | (1L << 5) | (1L << 4)  )
    csrrc   zero, sstatus, t0	//clear SSTATUS_SIE,SSTATUS_UIE,SSTATUS_SPIE,SSTATUS_UPIE
    li      t0, (1L << 8)
    csrrs   zero, sstatus, t0 	//set spp bit to supervisior-mode
    li      t0, ((1L << 9) | (1L << 5) | (1L << 1))
    csrw    sie, t0 //enable SIE_EIE SIE_TIE SIE_SIE
    call start_kernel
1:
    /* TODO Support for SMP BOOT*/
    wfi
    j 1b
2:  
    mv      tp, a0 // let tp to store hartid
    la      t0, dtb
    mv      s0, t0 // let s0 to store dtb address
    la      t0, sys_stack
    li      t1, CONFIG_RISCV_INIT_STACKSIZE
    addi    t2, tp, 1
    mul     t1, t1, t2
    add     sp, t0, t1
    // set csrs
    li      t0, ((1L << 1) | (1L << 0) | (1L << 5) | (1L << 4)  )
    csrrc   zero, sstatus, t0   //clear SSTATUS_SIE,SSTATUS_UIE,SSTATUS_SPIE,SSTATUS_UPIE
    li      t0, (1L << 8)
    csrrs   zero, sstatus, t0   //set spp bit to supervisior-mode
    li      t0, ((1L << 9) | (1L << 5) | (1L << 1))
    csrw    sie, t0 //enable SIE_EIE SIE_TIE SIE_SIE
    mv      a0, a1
    call    setup_satp
    call    start_kernel
1:

    j 1b
HEADFUNC_END(_start)

HEADFUNC_START_ALIGN(setup_satp,4)
     la   t0, _start_kernel
     li   t1, KERNEL_OFFSET
     sub  t0, t1, t0
     add  ra, t0, ra  //fix ra address
     add  sp, t0, sp  //fix sp address
     add  s0, t0, s0  //fix dtb address
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

DEFINE_DATA_START_ALIGN(dtb,.rvdtb,ASM_GLOBAL,PAGE_ALIGN)
    .space RISCV_DTB_SIZE
DEFINE_DATA_END(dtb)
