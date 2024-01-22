#include "Los_type.h"
#include "Los_err.h"
#include "platform.h"
#include "arch_port.h"
//上面是依赖 [依赖哪些公共库和哪些模块]
//下面是实现 [实现了哪些公共库或者哪些模块]
#include "mtime_trap.h"

uint64 m_scratch[3];
// 0 - t0
// 1 - sp
// 2 - t1


uint8  m_stack_sp[1024]__attribute__((aligned(16)));


void mtime_handler()
{
    uint64 tick_next = clint_get_tick();
    uint64 hart = r_mhartid();
    tick_next += (CPU_FREQUENCE/10); // tick per 0.1s
    clint_set_tcmp(hart,tick_next);
    w_sip(r_sip()|SSIP_S);
}