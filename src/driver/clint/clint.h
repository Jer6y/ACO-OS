/*
 *
 * CLINT 相关的板载驱动函数
 *
 */

#ifndef CLINT_H
#define CLINT_H

uint64 clint_get_tick();

uint64 clint_set_tcmp(uint32 hart, uint64 tick)



#endif