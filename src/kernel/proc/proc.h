/*
 *  进程相关的函数定义
 *  执行流建模
 */
#ifndef PROC_H
#define PROC_H

#define MAX_PROC_NAME_SIZE  20

typedef uint32  pstate_t;

typedef uint64  pid_t;

typedef uint32  prio_t;

typedef struct proc_s proc_t;

#define PROC_READY      (1u<<0)

#define PROC_RUNNING    (1u<<1)

#define PROC_PENDING    (1u<<2)

#define PROC_ON_WAIT    (1u<<3)

#define PROC_ZOMBIE     (1u<<4)

struct proc_s {
    uint64*     sp;             //栈顶指针       
    los_type_t  obj_type;
    pid_t       pid;
    prio_t      prio;
    pstate_t    state;
    uint64*     stack_base;     //栈基址
    uint64      stack_size;     //栈大小
    // 放置的就绪队列的 list
    mln_list_t ready_node;
    // 被 mutex 阻塞放置 list
    mln_list_t pend_node;
    // 超时阻塞放置的 list
    tick_node_t time_node;
    // 信号量放置的 list
    mln_list_t  sem_node;
//  for debug
    char  proc_name[MAX_PROC_NAME_SIZE];    
} ;

proc_t* proc_alloc();

void  proc_free();



#endif