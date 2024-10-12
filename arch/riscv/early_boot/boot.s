
.align 4
.section .text
.extern main
.extern early_stack
.global _start
_start:
    la   t0,early_stack
    li   t1,8192
    add  t0,t0,t1
    add  sp,t0,zero
    call main
__spin:
    j __spin
