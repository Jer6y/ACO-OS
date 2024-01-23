#include "Los_type.h"
#include "Los_err.h"
#include "platform.h"
#include "stdarg.h"
#include "string.h"
#include "uart.h"
#include "riscv.h"
#include "plic.h"
#include "clint.h"
#include "spinlock.h"
#include "assert.h"
#include "memory.h"

extern void plic_init();
extern void plic_init_hart();
int os_start =0;


void entry_hathre()
{
    while(1)
    {
        asm volatile("wfi");
    }
}

void main()
{
    if(r_tp() == 0)
    {
        uartinit();
        plic_init();
        plic_init_hart();
        module_memory_init();
    }
    else 
    {
        plic_init_hart();
    }
    while(os_start != 1);
    while(1)
    {
        asm volatile("wfi");
    }
}