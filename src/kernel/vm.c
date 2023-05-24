#include <def.h>
#include <platform.h>
#include <vm.h>
extern char textEnd[];
extern char end[];
pagetable* kernel_pgt;
void kvm_init()
{
    if((kernel_pgt = (pagetable*)kalloc())==0)
        panic("kvm_init:kernel_pgt can't reach!\n");
    kvm_map(kernel_pgt,KERNEL_BASE,KERNEL_BASE,(uint64)textEnd,PTE_R|PTE_X);
    kvm_map(kernel_pgt,(uint64)textEnd,(uint64)textEnd,(uint64)end+PAGESIZE,PTE_R|PTE_W);
    kvm_map(kernel_pgt,(uint64)end+PAGESIZE,(uint64)end+PAGESIZE,RAM_END+PAGESIZE,PTE_R|PTE_W);
}
void kvm_flush()
{
    sfence();
    w_satp(MAKESATP(kernel_pgt));
    sfence();
}
void kvm_map(pagetable* table,uint64 v_ad,uint64 p_ad,uint64 p_end,int perm)
{
    if(mmap(table,v_ad,p_ad,p_end,perm)==-1)
    panic("kvmmap:map error!\n");
}

int mmap(pagetable* table,uint64 v_ad,uint64 p_ad,uint64 p_end,int perm)
{
    uint64 v_st = PAGEDOWN(v_ad);
    uint64 p_st = PAGEDOWN(p_ad);
    p_end  = PAGEDOWN(p_end);
    for(;p_st<p_end;v_st+=PAGESIZE,p_st+=PAGESIZE)
    {
        pte_t* pte;
        if((pte=walk(table,v_st,1))==0) return -1;
        if(*pte & PTE_V)  
        {
            panic("mmap:remap!\n");
        }
        (*pte) = (pte_t)PA2PTE(p_st);
        (*pte) |= (perm | PTE_V);
    }
    return 0;
}
pte_t* walk(pagetable* table,uint64 v_ad,int is_aloc)
{
    if(v_ad>MAX_VA)  panic("walk:over MAX_VA!\n");
    pagetable* pgt = table;
    for(int i=2;i>0;i--)
    {
        pte_t* pte;
        int index = PX(i,v_ad);
        pte = &(pgt[index]);
        if((*pte)&PTE_V)
        {
            pgt = (pagetable*)PTE2PA(*pte) ;
        }
        else
        {
            if(!is_aloc||((pgt =(pagetable*)kalloc())==0))
                return 0;
            *pte = PA2PTE(pgt)|PTE_V;
        }
    }
    return &(pgt[(int)PXF(0,v_ad)]);
}