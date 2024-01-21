#include "Los_type.h"
#include "Los_err.h"
//上面是依赖 [依赖哪些公共库和哪些模块]
//下面是实现 [实现了哪些公共库或者哪些模块]
#include "bsp_port.h"

#define CLINT_MTIMECMP(hartid)      (CLINT + 0x4000 + 8*(hartid))
#define CLINT_MTIME                 (CLINT + 0xBFF8)

uint64 clint_get_tick()
{
    return *((uint64 *)CLINT_MTIME);
}

uint64 clint_set_tcmp(uint32 hart, uint64 tick)
{
    *(uint64*)CLINT_MTIMECMP(hart) = tick;
}