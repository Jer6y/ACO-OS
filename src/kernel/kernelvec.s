
.section .text
.global timer_vec
.extern tick_passed
timer_vec:
    csrrw a0,mscratch,a0
    sd  a1,0(a0)
    sd  a2,8(a0)
    sd  a3,16(a0)
    ld  a3,24(a0)
    ld  a1,32(a0)
    ld  a2,0(a3)
    add a1,a2,a1
    sd  a1,0(a3)
    # addi tp,tp,1
    add a1,zero,2
    csrw sip,a1
    la  a1,tick_passed
    ld  a2,0(a1)
    li  a3,1
    add a2,a2,a3
    sd  a2,0(a1)
    ld  a3,16(a0)
    ld  a2,8(a0)
    ld  a1,0(a0)
    csrrw a0,mscratch,a0
    mret
.global kernel_vec
kernel_vec:
    