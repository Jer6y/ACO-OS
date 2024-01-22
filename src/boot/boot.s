
.extern ks_stack
.extern start
.section .text.start
.global _start
_start:
    csrr tp,mhartid
    la   sp,ks_stack
    add  t0,tp,1
    li   t1,4096
    mul  t0,t0,t1
    add  sp,t0,sp
    call start
