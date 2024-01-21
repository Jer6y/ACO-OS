#include <os.h>
struct proc* alloc_proc(struct proc* old,os_error* err)
{
    if(old != run_proc || old->magic_number != 0xaa) panic("proc:fork don't get the running proc");
    struct proc* new = (struct proc*)kalloc(err);
    if((*err) != OS_ERROR_DONE) return 0;
    
    new->proc_father    = old;
    new->block_next     = NULL;
    new->block_pre      = NULL;
    new->ready_next     = ready_start_o;
    new->ready_pre      = ready_end_o;
    new->ker_regst      = old->ker_regst;
    new->usr_regst      = old->usr_regst;
    new->proc_state     = PROC_READY;
    new->in_kernel      = 0;
    new->sleep_reason   = NULL;
    new->tick_passed    = 0;
    new->pid            = alloc_pid();
    new->magic_number   = old->magic_number;
    new->k_stack        = ((uint64)new + PAGESIZE);
    pagetable* table = uvm_alloc(err);
    if((*err) != OS_ERROR_DONE)
    {
        (*err) = OS_ERROR_PROC_NOROOMFPGT;
        free_proc(new,err);
        return 0;
    }
    
    mmap(table,TRAPFRAME,(uint64)(user_vec),(uint64)(user_vec)+PAGESIZE,PTE_R|PTE_X,err);
    if((*err) != OS_ERROR_DONE )
    {
        (*err) = OS_ERROR_PROC_NOROOMFPTE;
        free_proc(new,err);
        return 0;
    }

    uint64 t = (uint64)kalloc(err);
    if((*err) != OS_ERROR_DONE)
    {
        free_proc(new,err);
        (*err) = OS_ERROR_PROC_NOROOMFPTE;
        return 0;
    }
    mmap(table,TRAPFRAME,t,t+PAGESIZE,PTE_R|PTE_X,err);
    
    for(int i )

}
void  free_proc(struct  proc* this,os_error* err)
{
    if((this->magic_number) != 0xaa) panic("free a area not proc");
    if(this->usr_pgt !=0)
    {
        free_pagetable(this->usr_pgt);
        kfree((uint64)(this->usr_pgt),err);
        if((*err) !=OS_ERROR_DONE) panic("free_proc:don't get correct pgt");
    }
    kfree((uint64)this,err);
    if((*err) != OS_ERROR_DONE) panic("free_proc:don't get a correct proc");
    (*err) = OS_ERROR_DONE;
}
void  sleep_proc(struct proc* this,os_error*)
{

}
void  wakup_proc(void* reason,os_error*)
{

}