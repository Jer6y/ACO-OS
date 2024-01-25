/*
 * port to user
 * proc模块
 *
 */
#ifndef LOS_PROC_H
#define LOS_PROC_H

#define MAX_TASK_PRIO 64

#define MAX_PROC_NAME_SIZE  20

typedef void (*pexc_entry)(void* param);

typedef uint32  pstate_t;

typedef uint64  pid_t;

typedef uint32  prio_t;

typedef struct proc_s proc_t;

typedef struct proc_info_s proc_info_t;

#define PROC_READY      (1u<<0)

#define PROC_RUNNING    (1u<<1)

#define PROC_PENDING    (1u<<2)

#define PROC_ON_WAIT    (1u<<3)

#define PROC_ZOMBIE     (1u<<4)

#define PROC_ALLOC_SYS  (1u<<0)

#define PROC_ALLOC_USR  (1u<<1)

struct proc_s {
    uint64*     sp;             //栈顶指针       
    los_type_t  obj_type;
    prio_t      prio;
    pstate_t    state;
    uint8       alloc_flag;    
    uint64*     stack_base;     //栈基址
    uint64      stack_size;     //栈大小
    spinlock_t  proc_lock;      //这个lock 只会锁Signal 相关的变量
    //进程之间通信 如KILL 进程  挂起进程 然进程睡觉之类的全部都通过Signal实现
    pexc_entry  text_entry;
    //参数
    void*       param;
    // 放置的就绪队列的 list
    mln_list_t  ready_node;
    // 被阻塞放置 list
    mln_list_t  pend_node;
    // 超时阻塞放置的 list
    mln_list_t  time_node;
    // 信号量放置的 list
    mln_list_t  sem_node;
    // for debug
    char  proc_name[MAX_PROC_NAME_SIZE];    
} ;

typedef struct proc_info_s {
    uint64* stack_base; 
    uint64  stack_size;
    pexc_entry tex_entry;
    void*   param;
    prio_t  prio;
} proc_info_t;

LOS_ERR_T proc_create(proc_info_t* info, proc_t* proc_sd_in);

LOS_ERR_T proc_create_dyn(proc_t** proc_sd_in);

LOS_ERR_T proc_kill(proc_t* proc);

LOS_ERR_T proc_suspend(proc_t* proc);

LOS_ERR_T proc_resume(proc_t* proc);

LOS_ERR_T proc_delay(proc_t* proc);

#endif