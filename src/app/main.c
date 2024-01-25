#include "Los.h"

extern void plic_init();
extern void plic_init_hart();
extern void sched_init();
extern void first_sched();
extern void s_trap();
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
    while(1)
    {
        asm volatile("wfi");
    }
}
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
    }
    first_sched();
    while(1)
    {
        asm volatile("wfi");
    }
}