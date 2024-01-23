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

void plic_init()
{
    plic_setprio(UART0_IRQ,1);
}
void plic_init_hart()
{
    plic_enable(CPU_MODE_S,r_tp(),UART0_IRQ);
}