/*
 * 存放中断平台的板载驱动代码
 * 
 */
#ifndef PLIC_H
#define PLIC_H

#define MAX_PLIC_IRQ_NUM    1023

#define MAX_PLIC_PRI_NUM    8

LOS_ERR_T plic_claim(uint32 hart, uint32 mode, uint32* irq_dev);

LOS_ERR_T plic_complete(uint32 hart, uint32 mode, uint32 dev);

LOS_ERR_T plic_enable(uint32 mode, uint32 hart, uint32 dev);

LOS_ERR_T plic_disable(uint32 mode, uint32 hart, uint32 dev);

LOS_ERR_T plic_setprio(uint32 dev, uint8 prio);

LOS_ERR_T plic_getprio(uint32 dev, uint32* prio_dev);

#endif
