#include <os.h>
#include <os_kernel_dt.h>
extern pagetable* kernel_pgt;

void main()
{
    uart_init();
    plic_init();
    plic_init_hart();

    kinit();
    kvm_init();
    kvm_flush_hart();

    os_error err;
    pagetable* a = uvm_alloc(&err);
    pagetable* b = uvm_alloc(&err);
    uint64 c = kalloc(&err);
    uvm_map(a,0,c,c+PAGESIZE,PTE_R,&err,0);
    uvm_map(b,0,c,c+PAGESIZE,PTE_R,&err,1);

    free_pagetable(a);
    free_pagetable(b);
    kfree(a,&err);
    kfree(b,&err);
    while(1);
    return;
}
