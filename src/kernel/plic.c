#include <plic.h>
#include <os.h>

void plic_init()
{
    *(uint32 *)PLIC_PRIORITY(UART_IRQ)=1;
}
void plic_init_hart()
{
    uint64 hart_id = r_tp();
    //设置该hart的plic中断使能
    *(uint32 *)PLIC_SENABLE(hart_id)=(1<<UART_IRQ);
    //设置该hart的plic的阈值为0
    *(uint32 *)PLIC_SPRIORITY(hart_id)=0;
}
uint32 plic_claim()
{
    uint32 x;
    uint64 hart_id =r_tp();
    x = *(uint32*)(PLIC_SCLAIM(hart_id));
    return x;
}
void plic_complete(uint32 IID)
{
    uint64 hart_id =r_tp();
    *((uint32*)PLIC_SCOMPLETE(hart_id))=IID;
}