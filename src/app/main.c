#include "Los.h"

extern void plic_init();
extern void plic_init_hart();
extern void sched_init();
extern void first_sched();
extern void s_trap();
extern void zombie_update();
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
void timer_hand(void* param)
{
    printk("hello, timer_hand! once \n");   
    timer_t tid;
    timer_info_t t_info;
    t_info.handler = timer_hand;
    t_info.attr = TIMER_ONCE ;
    t_info.param = 0;
    t_info.tick = 10;
    timer_register(&tid, &t_info);
    timer_unregister(0);
}

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
    }
}

void print_hello(void* param)
{
    int i=0;
    while(1)
    {
        for(int i=0;i<10;i++)
        {
            printk("[CPU %d ]: hello,world %d\n",r_tp(),i);
            asm volatile("wfi");
        }
        proc_kill(0);
    }
}

proc_t* poc_tst;

extern void proc_init();
void main()
{
    if(r_tp() == 0)
    {
        uartinit();
        plic_init();
        plic_init_hart();
        module_memory_init();
        sched_init();
        proc_init();
        timer_init();
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
        proc_create_dyn(&poc_tst,3,print_hello,NULL);
        timer_t tid;
        timer_info_t t_info;
        t_info.handler = NULL;
        t_info.attr = 0;
        t_info.param = 0;
        t_info.tick = 10;
        timer_register(&tid, &t_info);
        
        t_info.handler = timer_hand;
        t_info.attr = 0;
        t_info.param = 0;
        t_info.tick = 10;
        timer_register(&tid, &t_info);

        t_info.handler = timer_hand;
        t_info.attr = TIMER_ONCE;
        t_info.param = 0;
        t_info.tick = 0;
        timer_register(&tid, &t_info);
    
        
        t_info.handler = timer_hand;
        t_info.attr = TIMER_ONCE | TIMER_ALWAYS;
        t_info.param = 0;
        t_info.tick = 10;
        timer_register(&tid, &t_info);

        t_info.handler = timer_hand;
        t_info.attr = TIMER_ONCE ;
        t_info.param = 0;
        t_info.tick = 10;
        timer_register(&tid, &t_info);
    
    
    }
    first_sched();
    while(1)
    {
        asm volatile("wfi");
    }
}