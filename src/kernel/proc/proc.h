/*
 *  进程相关的函数定义
 *  执行流建模
 */
#ifndef PROC_H
#define PROC_H

#include "Los_proc.h"

#define TASK_PRIO_ILLEAGEL(x)  (!((x)>=1 && (x)< MAX_TASK_PRIO))

extern proc_t* running_proc[CPUS];
#define RUNNING_PROC running_proc[r_tp()]

void proc_init();

void zombie_update();

void proc_on_zombie(proc_t* proc); //一旦挂上zombie 不可能再恢复了

#endif