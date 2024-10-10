/*
 *  进程通信信号
 *
 *  
 */
#ifndef LOS_SIGNAL_H
#define LOS_SIGNAL_H


#define SIG_KILL       (1u<<0)    //优先级依次往下 高优先级优先执行 
#define SIG_RESUME     (1u<<2)
#define SIG_PEND       (1u<<1)
#define SIG_DELAY      (1u<<3)

// 上面的信号都会影响进程状态 下面可以自定义一些信号


LOS_ERR_T signal_proc(proc_t* proc, sig_t signal);

LOS_ERR_T designal_proc(proc_t* proc, sig_t signal);


#endif