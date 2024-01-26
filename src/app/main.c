#include "Los.h"

extern void plic_init();
extern void plic_init_hart();
extern void sched_init();
extern void first_sched();
extern void s_trap();
extern void zombie_update();
extern void pend_update();
extern void timer_init();
int os_start =0;
int os_end = 0;
spinlock_t lk;
// tick_node_t timer_node;
// int timer_test =0;
// int test_handler(tick_node_t* tick_node)
// {
//     printk("timer_test : %d\n",timer_test++);
//     return 0;
// }
// proc_t* tmp[CPUS][400];
// extern void proc_init();
// extern void proc_dump();
// extern void dump_proc_mem();

void entry_hathre()
{
    while(os_start != 1) 
    {
        asm volatile("wfi");
    }
    while(1)
    {
        asm volatile("wfi");
        zombie_update();
        // pend_update();
    }
}

mutex_t Mutex;
int a ;

void print_hello(void* param)
{
    int i=0;
    for(int i=0;i<1000;i++)
    {
        mutex_lock(&Mutex);
        a++;
        mutex_unlock(&Mutex);
    }
    while(1)
    {
        //lock_scheduler();
        ASSERT(proc_delay(0) == ERR_NONE);
        // if(i == 0 )
        // {

        // }
        printk("[CPU %d ]: hello %d\n",r_tp(),i++);   
        //unlock_scheduler();
    }
}


void print_world(void* param)
{
    int i=0;
    for(int i=0;i<1000;i++)
    {
        mutex_lock(&Mutex);
        a++;
        mutex_unlock(&Mutex);
    }
    while(1)
    {
        //lock_scheduler();
        ASSERT(proc_delay(0) == ERR_NONE);
        // if(i == 0 )
        // {

        // }
        printk("a : %d \n",a);
        printk("[CPU %d ]: world %d\n",r_tp(),i++);   
        //unlock_scheduler();
    }
}


proc_t* poc_tst;
proc_t* poc_tst_2;

extern void proc_init();
void main()
{
    if(r_tp() == 0)
    {
        uartinit();
        plic_init();
        plic_init_hart();
        module_memory_init();
        timer_init();
        sched_init();
        proc_init();
        init_lock(&lk,"lk");
        os_start = 1;
    }
    else 
    {
        plic_init_hart();
    }
    w_stvec((uint64)s_trap);
    while(os_start != 1);
    lock(&lk);
    os_end++;
    unlock(&lk);
    if(r_tp()==0)
    {
        while(os_end != 3) ;
        printk("end\n");
        mutex_init(&Mutex,MUTEX_ATTR_BLOCK);
        proc_create_dyn(&poc_tst,3,print_hello,NULL);
        proc_create_dyn(&poc_tst_2,3,print_world,NULL);
        
    }
    first_sched();
    while(1)
    {
        asm volatile("wfi");
    }
}