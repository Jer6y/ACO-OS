.align 4
.section .text
.global tsk_contex_load
.global tsk_trap
tsk_contex_load:
    ld  t0, 0(a0)
    ld  t1, 0(t0)
    csrw sepc, t1
    ld  t1, 8(t0)
    csrw sstatus, t1
    ld  t6, 16(t0)
    ld  t5, 24(t0)
    ld  t4, 32(t0)
    ld  t3, 40(t0)
    ld  s11, 48(t0)
    ld  s10, 56(t0)
    ld  s9, 64(t0)
    ld  s8, 72(t0)
    ld  s7, 80(t0)
    ld  s6, 88(t0)
    ld  s5, 96(t0)
    ld  s4, 104(t0)
    ld  s3, 112(t0)
    ld  s2, 120(t0)
    ld  a7, 128(t0)
    ld  a6, 136(t0)
    ld  a5, 144(t0)
    ld  a4, 152(t0)
    ld  a3, 160(t0)
    ld  a2, 168(t0)
    ld  a1, 176(t0)
    ld  a0, 184(t0)
    ld  s1, 192(t0)
    ld  s0, 200(t0)
    ld  t2, 208(t0)
    ld  t1, 216(t0)
    # ld  tp, 232(t0)
    ld  gp, 240(t0)
    ld  ra, 248(t0)
    addi sp, t0, 256
    ld  t0, 224(t0)
    sret
tsk_trap:
    ecall
    ret
    