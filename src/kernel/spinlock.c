#include <def.h>

void init_lock(struct spinlock* lk,char *name)
{
    lk->cpu   =0;
    lk->value =0;
    lk->name  =name;
}

void lock(struct spinlock* lk)
{
    push_on();
    while(__sync_lock_test_and_set(&lk->value,1)) ;
    __sync_synchronize();
    lk->cpu = mycpu();
}
void unlock(struct spinlock* lk)
{
    lk->cpu =0;
    __sync_lock_release(&lk->value);
    __sync_synchronize();
    pop_off();
}
void push_on()
{
    int old = r_int();
    w_int_off();
    struct cpu* cpu = mycpu();
    if(cpu->int_depth==0)
    {
        cpu->int_enable = old;
    }
    cpu->int_depth++;
}
void pop_off()
{
    if(r_int())
    {
        panic("pop_off:int_on");
    }
    struct cpu* cpu = mycpu();
    if(cpu->int_depth<=0)
    {
        panic("pop_off:no depth");            
    }
    cpu->int_depth--;
    if(cpu->int_depth==0&&cpu->int_enable==1)
    w_int_on();
}