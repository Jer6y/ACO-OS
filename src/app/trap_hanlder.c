#include "Los.h"

#include "sched.h"
#define IS_INTERRUPT(x) (((x)&0x8000000000000000ull) != 0)
#define IS_SOFT_INT(x)  ((x) == 0x8000000000000001ull)

void clear_soft_pending()
{
    uint64 x = r_sip();
    x &= SSIP_R;
    w_sip(x);
}

void trap_handler(uint64 scause, uint64 stval, uint64 param)
{
    if(IS_INTERRUPT(scause))
    {
        if(IS_SOFT_INT(scause))
        {
            clear_soft_pending();
            SET_SCHED();
        }
        else 
        {
            
        }
    }
    else 
    {
        panic("happen exception");
    }
    return;
}