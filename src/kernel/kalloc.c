#include <os.h>

extern char end[];

void kinit()
{
    #ifdef KALLOC_SPINLOCK
    init_lock(&(kmem.lk),"ka_lock");
    #endif 
    for(int i =0;i<TOTALPAGES;i++) 
    bitmap[i] =1;
    kmem.next=NULL;
    kmem.page_nums=0;
    kmem.free_pages=0;
    kmem.page_size =PAGESIZE;
    kfree_arrange((uint64)end,RAM_END);
    kmem.page_nums = kmem.free_pages;

}

void* kalloc(os_error* err)
{
    void * p =NULL;
    
    #ifdef KALLOC_SPINLOCK
    lock(&(kmem.lk));
    #else 
    push_on();
    #endif 

    p = (void*)kmem.next;
    if(p)
    {
        kmem.next = ((struct PHRL*)p)->next;
        kmem.free_pages--;
        bitmap[(((uint64)p) - KERNEL_BASE)>>12]++;
    }

    #ifdef KALLOC_SPINLOCK
    unlock(&(kmem.lk));
    #else
    pop_off();
    #endif

    if(p) 
    {
        *err = OS_ERROR_DONE;
        memset(p,0,PAGESIZE);
    }
    else 
    {
        *err = OS_ERROR_KALOC_NOROOM;   
    }
    return p;
}

void kfree_arrange(uint64 start,uint64 end)
{
    uint64 pa_start = PAGEUP(start);
    uint64 pa_end   = PAGEDOWN(end);
    for(;pa_start<=pa_end;pa_start+=PAGESIZE)
    {
        os_error err;
        kfree(pa_start,&err);
        if(err != OS_ERROR_DONE)
        {
            panic("kinit failed");
            return;
        }
    }
}

void kfree(uint64 address,os_error* err)
{
    if(address&(PAGESIZE-1))
    {
        *err = OS_ERROR_KFREE_UNLIGN;
        return;
    }
    if(address < KERNEL_BASE || address >RAM_END)
        panic("kbit_index_add: not in range");
        
    #ifdef KALLOC_SPINLOCK
    lock(&(kmem.lk));
    #else
    push_on();
    #endif

    if(bitmap[((address - KERNEL_BASE)>>12)] ==0) panic("kfree:free a wrong page");
    bitmap[((address - KERNEL_BASE)>>12)] --;
    if(bitmap[((address - KERNEL_BASE)>>12)] ==0)
    {
        struct PHRL* tmp = kmem.next;
        kmem.next = (struct PHRL*)address;
        ((struct PHRL*)address)->next = tmp;
        kmem.free_pages++;
    }
    
    #ifdef KALLOC_SPINLOCK
    unlock(&kmem.lk);
    #else
    pop_off();
    #endif

    
    *err = OS_ERROR_DONE;
    return;
}

void kbit_index_add(uint64 index)
{
    if(index&(PAGESIZE-1))
        panic("kbit_index_add: not aligned");
    if(index < KERNEL_BASE || index >RAM_END)
        panic("kbit_index_add: not in range");
    #ifdef KALLOC_SPINLOCK
    lock(&(kmem.lk));
    #else
    push_on();
    #endif

    bitmap[((index - KERNEL_BASE)>>12)] ++;

    #ifdef KALLOC_SPINLOCK
    lock(&(kmem.lk));
    #else
    pop_off();
    #endif
}