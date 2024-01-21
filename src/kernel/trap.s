
.section .TRAPROOM
.global usrvec
.extern run_proc
.extern user_trap
usrvec:
    csrrw t0,sscratch,t0
    la  t0,trap_temp
    sd  t1,0(t0)
    la  t0,trap_kernel
    ld  t1,0(t0)
    csrw satp,t1
    sfence.vma zero,zero
    la  t0,trap_temp
    ld  t1,0(t0)
    la  t0,run_proc
    sd  ra,0(t0)
    sd  sp,8(t0)
    sd  gp,16(t0)
    sd  tp,24(t0)
    sd  t1,40(t0)
    sd  t2,48(t0)
    sd  t3,56(t0)
    sd  t4,64(t0)
    sd  t5,72(t0)
    sd  t6,80(t0)
    sd  s0,88(t0)  
    sd  s1,96(t0) 
    sd  s2,104(t0) 
    sd  s3,112(t0) 
    sd  s4,120(t0) 
    sd  s5,128(t0) 
    sd  s6,136(t0) 
    sd  s7,144(t0) 
    sd  s8,152(t0) 
    sd  s9,160(t0) 
    sd  s10,168(t0) 
    sd  s11,176(t0) 
    sd  a0,184(t0)
    sd  a1,192(t0)
    sd  a2,200(t0)
    sd  a3,208(t0)
    sd  a4,216(t0)
    sd  a5,224(t0)
    sd  a6,232(t0)
    sd  a7,240(t0)
    ld  tp,248(t0)
    add t1,t0,zero
    csrrw csrrw t0,sscratch,t0
    ld  t0,32(t1)
    call user_trap

.global trap_kernel
trap_kernel:
    .byte 0,0,0,0,0,0,0,0
.global trap_user
trap_user:
    .byte 0,0,0,0,0,0,0,0

.global trap_temp
trap_temp:
    .byte 0,0,0,0,0,0,0,0
.extern run_proc
.global kertrans
kertrans:
    csrrw sscratch,t0,sscratch
    la t0,trap_temp
    sd t1,0(t0)
    la t0,trap_kernel
    csrr  t1,satp
    sd t1,0(t0)
    la t0,run_proc
    ld t1,256(t0)
    la t0,trap_user
    sd t1,0(t0)
    csrw satp,t1
    sfence.vma zero,zero
    la t0,trap_temp
    ld t1,8(t0)
    csrrw sscratch,t0,sscratch
    sret
