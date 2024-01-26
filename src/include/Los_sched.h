/*
 *  scheduler 相关的PORT给用户的
 *
 */

#ifndef LOS_SCHED_H
#define LOS_SCHED_H



//========================check 调度器锁的状态API=================
int scheduler_locked();
int scheduler_unlocked();
//========================对调度器锁的状态修改API=================
LOS_ERR_T lock_scheduler();
LOS_ERR_T unlock_scheduler();
//===============================================================


#endif