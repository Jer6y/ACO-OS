/*
 * 调度器相关的函数内容
 */
#ifndef SCHED_H
#define SCHED_H

#include "Los_proc.h"

//=======================除了汇编部分,不要直接使用下面的数据结构===
extern int int_nest[CPUS];
extern int on_sched[CPUS];
//===========================使用下面的API=======================
#define IS_INT_NESTED()         (int_nest[r_tp()]!=0)
#define IS_INT_NOT_NESTED()     (int_nest[r_tp()]==0)
#define IS_INT_SCHED()          (on_sched[r_tp()]!=0)
#define IS_INT_NOT_SCHED()      (on_sched[r_tp()]==0)
#define INT_NEST_IN()           (int_nest[r_tp()]++)
#define INT_NEST_OUT()          {ASSERT(int_nest[r_tp()]>0);int_nest[r_tp()]--;}
#define SET_SCHED()             {ASSERT(on_sched[r_tp()]==0); on_sched[r_tp()]=1;}
#define SET_UN_SCHED()          {ASSERT(on_sched[r_tp()]==1); on_sched[r_tp()]=0;}

//===============================================================


// for strap.s
bool check_is_in_nest();



//========================check 调度器锁的状态API=================
int scheduler_locked();
int scheduler_unlocked();
//========================对调度器锁的状态修改API=================
LOS_ERR_T lock_scheduler();
LOS_ERR_T unlock_scheduler();
//===============================================================




//========================调度器其他API======================================================================================================
void sched_init();                                      //数据结构初始化
proc_t* sched_get_highest();                            //获得当前调度器中优先级最高的进程 无需上锁 hardware-thread-safety API
void sched();                                           //调度函数 没有出口 只能在软件中断或者特殊的异常中调用
void sched_add_in(proc_t* proc);                        //把进程放到就绪队列内部
void first_sched();
//===========================================================================================================================================


#endif