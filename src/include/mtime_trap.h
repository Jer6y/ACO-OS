/*
 * m模式下 time trap 的入口handler函数
 */
#ifndef MTIME_TRAP_H
#define MTIME_TRAP_H


extern void mtime_trap();
// 你应该把 mtvec的值填为 mtime_trap

extern uint64 m_scratch[3];
// 将mscratch的值甜味 m_scratch的地址

#endif



