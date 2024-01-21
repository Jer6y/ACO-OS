#include <os.h>

extern char textEnd[];
extern char end[];

void kvm_init()
{
    os_error err;
    kernel_pgt = (pagetable*)kalloc(&err);
    if(err != OS_ERROR_DONE) 
    panic("kvm_init:kernel_pgt can't alloc!");

    kvm_map(kernel_pgt,KERNEL_BASE,KERNEL_BASE,(uint64)textEnd,PTE_R|PTE_X);
    kvm_map(kernel_pgt,(uint64)textEnd,(uint64)textEnd,(uint64)end+PAGESIZE,PTE_R|PTE_W);
    kvm_map(kernel_pgt,(uint64)end+PAGESIZE,(uint64)end+PAGESIZE,RAM_END+PAGESIZE,PTE_R|PTE_W);
    kvm_map(kernel_pgt,UART_START,UART_START,UART_START+PAGESIZE,PTE_R|PTE_W);
    kvm_map(kernel_pgt,PLIC_START,PLIC_START,UART_START,PTE_R|PTE_W);
}

void kvm_flush_hart()
{
    sfence();
    w_satp(MAKESATP(kernel_pgt));
    sfence();
}
void kvm_map(pagetable* table,uint64 v_ad,uint64 p_ad,uint64 p_end,int perm)
{
    os_error err;
    mmap(table,v_ad,p_ad,p_end,perm,&err,0);
    if(err != OS_ERROR_DONE)
    panic("kvm_map: kernel_pgt map failed!");

}

void uvm_map(pagetable* table,uint64 v_ad,uint64 p_ad,uint64 p_end,int perm,os_error* err,int is_adindex)
{
    mmap(table,v_ad,p_ad,p_end,perm,err,is_adindex);
}
// map   <=> v_ad -> p_ad
// lenth <=> [page at p_ad , page at p_end)
void mmap(pagetable* table,uint64 v_ad,uint64 p_ad,uint64 p_end,int perm,os_error* err,int is_adindex)
{
    uint64 v_st = PAGEDOWN(v_ad);
    uint64 p_st = PAGEDOWN(p_ad);
    p_end  = PAGEDOWN(p_end);
    for(;p_st<p_end;v_st+=PAGESIZE,p_st+=PAGESIZE)
    {
        pte_t* pte;
        if((pte=walk(table,v_st,1,err))==0) 
        {
            //*err =OS_ERROR_AL_NOROOM;
            return ;
        }
        if(*pte & PTE_V)  
        {
            *err = OS_ERROR_VM_REMAP;
            return;
        }
        (*pte) = (pte_t)PA2PTE(p_st);
        if(is_adindex) kbit_index_add(p_st);
        (*pte) |= (perm | PTE_V);
    }
    *err = OS_ERROR_DONE;
    return ;
}

pte_t* walk(pagetable* table,uint64 v_ad,int is_aloc,os_error* err)
{
    if(v_ad>MAX_VA)  panic("walk:over MAX_VA!");
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
            if(!is_aloc||((pgt =(pagetable*)kalloc(err))==0))
            {
                if(is_aloc) *err = OS_ERROR_KALOC_NOROOM;
                else        *err = OS_ERROR_VM_AL_NOTALLOWED;
                return 0;
            }
            *pte = PA2PTE(pgt)|PTE_V;
        }
    }
    *err =OS_ERROR_DONE;
    return &(pgt[(int)PX(0,v_ad)]);
}

static void free_pte_rec(pagetable* table,int level)
{
    if(level==2)
    {
        for(int i=0;i<512;i++)
        {
            if(table[i] & PTE_V )
            {
                os_error err;
                kfree(PTE2PA(table[i]),&err);
                if(err != OS_ERROR_DONE) 
                {
                    panic("PTE2PA FAULT!");
                    return;
                }
            }
        }
    }
    else
    {
        for(int i=0;i<512;i++)
        {
            if((table[i] & PTE_W)||(table[i] & PTE_R)||(table[i]& PTE_X))
            { panic("free_table:give the error table!");}
            else if(table[i] & PTE_V)
            {
                free_pte_rec((pagetable* )PTE2PA(table[i]),level+1);
            }
        }
    }
}

void free_pte(pagetable* table)
{
    free_pte_rec(table,0);
}


static void free_pgt_rec(pagetable* table,int level)
{
    if(level==1)
    {
        for(int i=0;i<512;i++)
        {
            if(table[i] & PTE_V )
            {
                os_error err;
                kfree(PTE2PA(table[i]),&err);
                if(err != OS_ERROR_DONE)
                {
                    panic("PTE2PA FAULT!");
                    return;
                }
            }
        }
        return;
    }
    for(int i=0;i<512;i++)
    {
        if(table[i] & PTE_V)
        {
            free_pgt_rec((pagetable*)PTE2PA(table[i]),level+1);
            os_error err;
            kfree(PTE2PA(table[i]),&err);
            if(err != OS_ERROR_DONE)
            {
                panic("PTE2PA FAULT!");
                return;
            }
        }
    }
    return;
}

void free_pagetable(pagetable* table)
{
    free_pte(table);
    free_pgt_rec(table,0);
}


pagetable* uvm_alloc(os_error* err)
{
    pagetable* pgt = (pagetable*)kalloc(err);
    if(*err != OS_ERROR_DONE)
        return 0;
    return pgt;
}

uint64 walkadrs(pagetable* table,uint64 v_ad,os_error* err)
{
    uint64 virtual = (v_ad & ~(PAGESIZE-1));
    pte_t* t = walk(table,virtual,0,err);
    if(*err ==OS_ERROR_DONE)
    {
        if((*t) & PTE_V)
        {
            *err = OS_ERROR_DONE;
            return (uint64)(PTE2PA(*t) | (v_ad & (PAGESIZE-1)));
        }
    }
    *err = OS_ERROR_VM_CANT_FIND;
    return 0;
}

void  unmap(pagetable* table,uint64 v_ad,size_t pg_size,os_error* err)
{
    uint64 virtual = (v_ad & ~(PAGESIZE-1));
    for(size_t i=0;i<pg_size;i++,virtual+=PAGESIZE)
    {
        pte_t* t =walk(table,virtual,0,err);
        if(*err ==OS_ERROR_DONE)
        {
            if((*t) & PTE_V)
            {
                kfree((uint64)PTE2PA(*t),err);
                if(*err !=OS_ERROR_DONE)
                {
                    panic("PTE2PA FAULT!");
                }
                *t =0;
            }
            else
            {
                if(i==0)
                {
                    *err = OS_ERROR_VM_UNMAP_TOFAIL;
                    return;
                }
                else 
                {
                    *err = OS_ERROR_VM_UNMAP_PTFAIL;
                    return;
                }
            }
        }
        else
        {
            if(i==0)
            {
                *err = OS_ERROR_VM_UNMAP_TOFAIL;
                return;
            }
            else 
            {
                *err = OS_ERROR_VM_UNMAP_PTFAIL;
                return;
            }
        }
    }
    *err = OS_ERROR_DONE;
    return;
}