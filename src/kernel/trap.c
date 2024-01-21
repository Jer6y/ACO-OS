#include <os.h>
#include <riscv.h>
#include <plic.h>

#define INTERUPT 0x8000000000000000ull

void trap_init()
{
    //stvec
    w_stvec((uint64)kernel_vec);
}
void kernel_trap()
{
    assert((r_int())==0);
    assert(((r_sstatus()&SPP)==SPP_S));
    uint64 sstatus = r_sstatus();
    uint64 spec    = r_sepc();
    uint64 scause  = r_scause();
    
    if(scause&INTERUPT)
    {
        if((scause&(~INTERUPT))==9)
        {
            uint32 irq = plic_claim();
            switch(irq)
            {
                case UART_IRQ:
                uart_int_handler();
                break;
            }
            plic_complete(irq);
        }
        else if((scause&(~INTERUPT))==1)
        {
            printf("timer!\n");
            w_sip(r_sip()&(~2));
        }
    }
    else
    {
        uint64 stval = r_stval();
        printf("scause:%p stval:%p",scause,stval);
        panic("exception!");
    }
    
}