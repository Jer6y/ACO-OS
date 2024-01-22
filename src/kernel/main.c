#include "Los_type.h"
#include "Los_err.h"
#include "bsp_port.h"
#include "arch_port.h"
#include "string.h"
#include "memory.h"
extern char end[];
m_pool_t mmpool_handler;

m_slab_t* slab_test;
int s = 0;
spinlock_t lk2;
int es =0;

void* tmp[CPUS][512] ={0};
int   order_tmp[CPUS][512] = {0};
        

void main()
{
    if(r_tp() == 0)
    {
        uartinit();
        init_lock(&lk2,"spinlock");
        m_pool_init(&mmpool_handler,(void*)end,(RAM_END - (uint64)end)+1);
        m_pool_dump(&mmpool_handler);
        void * mem = m_pool_alloc(&mmpool_handler,5);
        slab_test = m_slab_init_align(mem,5,237,16);
        dump_slab(slab_test);
        s = 1;
    }
    while(s != 1) ;
    while(1)
    {
        for(int i=0;i<50;i++)
        {
            tmp[r_tp()][i] = m_slab_obj_alloc(slab_test);
        }
        for(int i=0;i<50;i++)
        {
            if(tmp[r_tp()][i] != NULL)
            {
                m_slab_obj_free(slab_test,tmp[r_tp()][i]);
            }
        }
        for(int i=50;i<100;i++)
        {
            tmp[r_tp()][i] = m_slab_obj_alloc(slab_test);
        }
        for(int i=50;i<100;i++)
        {
            if(tmp[r_tp()][i] != NULL)
            {
                m_slab_obj_free(slab_test,tmp[r_tp()][i]);
            }
        }
        
        lock(&lk2);
        es++;
        unlock(&lk2);
        if(r_tp() == 0)
        {
            while(1)
            {
                lock(&lk2);
                if(es ==4)
                {
                    unlock(&lk2);
                    break;
                }
                unlock(&lk2);        
            }
            dump_slab(slab_test);
        }
        while(1)
        {
            asm volatile("wfi");
        }
    }
}