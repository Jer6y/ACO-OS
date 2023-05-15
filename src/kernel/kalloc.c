#include <def.h>
#include <platform.h>
extern char end[];

struct PHRL
{
    struct PHRL* next;
};
struct
{
    struct spinlock ka_lock;
    struct PHRL* next;
} kmem;

void kinit()
{
    init_lock(&kmem.ka_lock,"ka_lock");
    kmem.next=NULL;    
}
void kfree(uint64 address)
{
    if(address&(PAGESIZE-1))
    {
        ;
    }
    lock(&kmem.ka_lock);
    struct PHRL* tmp = kmem.next;
    kmem.next = (struct PHRL*)address;
    ((struct PHRL*)address)->next = tmp;
    unlock(&kmem.ka_lock);
}