.align 4
.extern check_is_in_nest
.extern running_proc
.extern ks_stack
.extern interrupt_in
.extern interrupt_out
.section .text 
.global s_trap
s_trap:
    addi sp, sp, -256
    sd  ra, 248(sp)
    sd  gp, 240(sp)
  # sd  tp, 232(sp)
    sd  t0, 224(sp)
    sd  t1, 216(sp)
    sd  t2, 208(sp)
    sd  s0, 200(sp)
 # 使用abi trick
    add s0, a0, zero
 # #########
    sd  s1, 192(sp)
    sd  a0, 184(sp)
    sd  a1, 176(sp)
    sd  a2, 168(sp)
    sd  a3, 160(sp)
    sd  a4, 152(sp)
    sd  a5, 144(sp)
    sd  a6, 136(sp)
    sd  a7, 128(sp)
    sd  s2, 120(sp)
    sd  s3, 112(sp)
    sd  s4, 104(sp)
    sd  s5, 96(sp)
    sd  s6, 88(sp)
    sd  s7, 80(sp)
    sd  s8, 72(sp)
    sd  s9, 64(sp)
    sd  s10, 56(sp)
    sd  s11, 48(sp)
    sd  t3, 40(sp)
    sd  t4, 32(sp)
    sd  t5, 24(sp)
    sd  t6, 16(sp)
    csrr t1, sstatus
    sd  t1, 8(sp)
    csrr t1, sepc
    sd  t1, 0(sp)
    # 判断是否嵌套中断
    call check_is_in_nest
    bne a0, zero, jump_store_tsk
    la  t0, running_proc
    li  t1, 8
    mul t1, t1, tp
    add t0, t1, t0
    ld  t0, 0(t0) # 获取running task 
    sd  sp, 0(t0) # 把 sp 存进去
    la   sp,ks_stack
    add  t0,tp,1
    li   t1,4096
    mul  t0,t0,t1
    add  sp,t0,sp
jump_store_tsk:
    call interrupt_in

    csrr a0, scause
    csrr a1, stval
    add  a2, s0, zero
    call trap_handler
    
    call interrupt_out
    
    # 判断是否嵌套
    call check_is_in_nest
    bne a0, zero, int_nest_load
    
    # 尝试唤起调度器 如果不是要求调度 直接返回
    call sched
    la  t0, running_proc
    li  t1, 8
    mul t1, t1, tp
    add t0, t1, t0
    ld  t0, 0(t0) # 获取running task 
    ld  sp, 0(t0)
int_nest_load:
    ld  t1, 0(sp)
    csrw sepc, t1
    ld  t1, 8(sp)
    csrw sstatus, t1
    ld  t6, 16(sp)
    ld  t5, 24(sp)
    ld  t4, 32(sp)
    ld  t3, 40(sp)
    ld  s11, 48(sp)
    ld  s10, 56(sp)
    ld  s9, 64(sp)
    ld  s8, 72(sp)
    ld  s7, 80(sp)
    ld  s6, 88(sp)
    ld  s5, 96(sp)
    ld  s4, 104(sp)
    ld  s3, 112(sp)
    ld  s2, 120(sp)
    ld  a7, 128(sp)
    ld  a6, 136(sp)
    ld  a5, 144(sp)
    ld  a4, 152(sp)
    ld  a3, 160(sp)
    ld  a2, 168(sp)
    ld  a1, 176(sp)
    ld  a0, 184(sp)
    ld  s1, 192(sp)
    ld  s0, 200(sp)
    ld  t2, 208(sp)
    ld  t1, 216(sp)
    # ld  tp, 232(sp)
    ld  t0, 224(sp)
    ld  gp, 240(sp)
    ld  ra, 248(sp)
    addi sp, sp, 256
    sret
    
    
    
    
    
    
    
    
    
    
    
    

   
    

    

    