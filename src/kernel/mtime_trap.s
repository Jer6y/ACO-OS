.align 4
.section .text
.global mtime_trap
.extern m_stack_sp
mtime_trap:
    csrrw t0, mscratch, t0
    sd sp, 8(t0)  # m_scratch[1] = $sp;
    sd t1, 16(t0) # m_scratch[2] = $t1;
    add t1, t0, zero
    csrrw t0, mscratch, t0
    sd t0, 0(t1)  # m_scratch[0] = t0;
    la sp, m_stack_sp
    addi sp, sp, 1024
    addi sp, sp, -112
    sd   t6, 0(sp)
    sd   t5, 8(sp)
    sd   t4, 16(sp)
    sd   t3, 24(sp)
    sd   a7, 32(sp)
    sd   a6, 40(sp)
    sd   a5, 48(sp)
    sd   a4, 56(sp)
    sd   a3, 64(sp)
    sd   a2, 72(sp)
    sd   a1, 80(sp)
    sd   a0, 88(sp)
    sd   t2, 96(sp)
    sd   ra, 104(sp)
    # save the caller register
    call mtime_handler
    ld   ra, 104(sp)
    ld   t2, 96(sp)
    ld   a0, 88(sp)
    ld   a1, 80(sp)
    ld   a2, 72(sp)
    ld   a3, 64(sp)
    ld   a4, 56(sp)
    ld   a5, 48(sp)
    ld   a6, 40(sp)   
    ld   a7, 32(sp)
    ld   t3, 24(sp)
    ld   t4, 16(sp)
    ld   t5, 8(sp)
    ld   t6, 0(sp)
    addi sp, sp, 112
    csrrw t2, mscratch, t2
    ld   t0, 0(t2)
    ld   sp, 8(t2)
    ld   t1, 16(t2)
    csrrw t2, mscratch, t2
    mret

    
    
    
    
    

