#include "Los_type.h"
#include "Los_err.h"
#include "platform.h"
#include "riscv.h"
//上面是依赖 [依赖哪些公共库和哪些模块]
//下面是实现 [实现了哪些公共库或者哪些模块]
#include "plic.h"


#define PLIC_PRIORITY           (PLIC + 0x0)
#define PLIC_PENDING            (PLIC + 0x1000)
#define PLIC_MENABLE(hart)      (PLIC + 0x2000 + (hart)*0x100)
#define PLIC_SENABLE(hart)      (PLIC + 0x2080 + (hart)*0x100)
#define PLIC_MPRIORITY(hart)    (PLIC + 0x200000 + (hart)*0x2000)
#define PLIC_SPRIORITY(hart)    (PLIC + 0x201000 + (hart)*0x2000)
#define PLIC_MCLAIM(hart)       (PLIC + 0x200004 + (hart)*0x2000)
#define PLIC_SCLAIM(hart)       (PLIC + 0x201004 + (hart)*0x2000)
#define PLIC_MCOMPLETE(hart)    (PLIC_MCLAIM(hart))
#define PLIC_SCOMPLETE(hart)    (PLIC_SCLAIM(hart))

#define PLIC_PRIO_ILLEAGEL(x)   (!((x)>=0 && (x)<=MAX_PLIC_PRI_NUM))
#define PLIC_IRQ_ILLEAGEL(x)    (!((x)>=0 && (x)<=MAX_PLIC_IRQ_NUM))



LOS_ERR_T  plic_claim(uint32 hart, uint32 mode, uint32* irq_dev)
{
    if(mode != CPU_MODE_M || mode != CPU_MODE_S)
    {
        return ERR_RISCV_CPU_TYPE;
    }
    if(irq_dev == NULL)
    {
        return ERR_PTR_NULL;
    }
    uint32 x;
    if(mode == CPU_MODE_M) 
    {
        x = *(uint32*)(PLIC_MCLAIM(hart));
    }
    else
    {
        x = *(uint32*)(PLIC_SCLAIM(hart));       
    }
    *irq_dev = x;
    return ERR_NONE;
}


LOS_ERR_T  plic_complete(uint32 hart, uint32 mode, uint32 dev)
{
    if(PLIC_IRQ_ILLEAGEL(dev)) 
    {
        return ERR_PLIC_DEV_TYPE;
    }
    if(mode != CPU_MODE_M || mode != CPU_MODE_S)
    {
        return ERR_RISCV_CPU_TYPE;
    }
    if(mode == CPU_MODE_M)
    {
        *(uint32*)(PLIC_MCOMPLETE(hart)) = dev;
    }
    else
    {
        *(uint32*)(PLIC_SCOMPLETE(hart)) = dev;
    }

}

LOS_ERR_T plic_enable(uint32 mode, uint32 hart, uint32 dev)
{
    if(PLIC_IRQ_ILLEAGEL(dev)) 
    {
        return ERR_PLIC_DEV_TYPE;
    }
    if(mode != CPU_MODE_M || mode != CPU_MODE_S)
    {
        return ERR_RISCV_CPU_TYPE;
    }
    if(mode == CPU_MODE_M) 
    {
        *((uint32 *)PLIC_MENABLE(hart) + (dev>>5)) |= (1<<(dev & 0b11111));
    }
    else
    {
        *((uint32 *)PLIC_SENABLE(hart) + (dev>>5)) |= (1<<(dev & 0b11111));
    }
    return ERR_NONE;
}

LOS_ERR_T plic_disable(uint32 mode, uint32 hart, uint32 dev)
{
    if(PLIC_IRQ_ILLEAGEL(dev)) 
    {
        return ERR_PLIC_DEV_TYPE;
    }
    if(mode != CPU_MODE_M || mode != CPU_MODE_S)
    {
        return ERR_RISCV_CPU_TYPE;
    }
    if(mode == CPU_MODE_M) 
    {
        *((uint32 *)PLIC_MENABLE(hart) + (dev>>5)) &= (~(1<<(dev & 0b11111)));
    }
    else
    {
        *((uint32 *)PLIC_SENABLE(hart) + (dev>>5)) &= (~(1<<(dev & 0b11111)));
    }
    return ERR_NONE;
}

LOS_ERR_T plic_setprio(uint32 dev,uint8 prio)
{
    if(PLIC_IRQ_ILLEAGEL(dev)) 
    {
        return ERR_PLIC_DEV_TYPE;
    }
    if(PLIC_PRIO_ILLEAGEL(prio))
    {
        return ERR_PLIC_DEV_PRIO;
    }   
    *(uint32* )(PLIC_PRIORITY + dev*4) = prio;
    return ERR_NONE;
}


LOS_ERR_T  plic_getprio(uint32 dev, uint32* prio_dev)
{
    if(PLIC_IRQ_ILLEAGEL(dev)) 
    {
        return ERR_PLIC_DEV_TYPE;
    }
    if(prio_dev == NULL)
    {
        return ERR_PTR_NULL;
    }
    uint32 x;
    x = *(uint32* )(PLIC_PRIORITY + dev*4);  
    *prio_dev = x;
    return ERR_NONE;
}
