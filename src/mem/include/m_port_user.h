#ifndef __M_PORT_USER
#define __M_PORT_USER

#include "Los_type.h"
#include "Los_err.h"
#include "platform.h"
#include "stdarg.h"
#include "string.h"
#include "uart.h"
#include "riscv.h"
#include "plic.h"
#include "clint.h"
#include "spinlock.h"
#include "assert.h"

//some function or definition should be configed by user
#define EXIT_WHEN_ERROR_HAPPEN(number)              panic("panic in mem module,return number : %d\n",number);

#define M_PRINTF_NORMAL(...)                        {                                  \
                                                    printk("=======info=========\n");  \
                                                    printk(__VA_ARGS__);               \
                                                    printk("====================\n");  \
                                                    }

#define M_PRINTF_WARN(...)                          {                                   \
                                                    printk("=======warn=========\n");   \
                                                    printk(__VA_ARGS__);                \
                                                    printk("====================\n");   \
                                                    }                  

#define M_PRINTF_ERROR(...)                         {                                   \
                                                    printk("=======erro=========\n");   \
                                                    printk(__VA_ARGS__);                \
                                                    printk("====================\n");   \
                                                    }

#define M_LOCK_T                                    spinlock_t

#define M_LOCK_INIT(lk,attr)                        init_lock(lk,attr)

#define M_LOCK(lk)                                  lock(lk)

#define M_UNLOCK(lk)                                unlock(lk)

#define M_ADDR_T                                    uint64


#if (!defined(EXIT_WHEN_ERROR_HAPPEN))
    #error "EXIT_WHEN_ERROR_HAPPEN NOT DEFINE BY USER"
#endif

#if (!defined(M_PRINTF_NORMAL))
    #error "M_PRINTF_NORMAL NOT DEFINE BY USER"
#endif

#if (!defined(M_PRINTF_WARN))
    #error "M_PRINTF_WARN NOT DEFINE BY USER"
#endif


#if (!defined(M_PRINTF_ERROR))
    #error "M_PRINTF_ERROR NOT DEFINE BY USER"
#endif

#if (!defined(M_LOCK_T))
    #error "M_LOCK_T NOT DEFINE BY USER"
#endif

#if (!defined(M_ADDR_T))
    #error "M_ADDR_T NOT DEFINE BY USER"
#endif

#if (!defined(M_LOCK_INIT))
    #error "M_LOCK_INIT NOT DEFINE BY USER"
#endif

#if (!defined(M_LOCK))
    #error "M_LOCK NOT DEFINE BY USER"
#endif

#if (!defined(M_UNLOCK))
    #error "M_UNLOCK NOT DEFINE BY USER"
#endif

#if (!defined(M_ADDR_T))
    #error "M_ADDR_T NOT DEFINE BY USER"
#endif

//this isn't what user need to care about
#define M_LEVEL_INFO      16
#define M_LEVEL_WARN      32
#define M_LEVEL_ERR       64
#define M_EXIT(number)    EXIT_WHEN_ERROR_HAPPEN((number))

#define M_DEBUG(level,...) { if(level == M_LEVEL_INFO) {M_PRINTF_NORMAL(__VA_ARGS__);} else if(level == M_LEVEL_WARN) { M_PRINTF_WARN(__VA_ARGS__);} else { M_PRINTF_ERROR(__VA_ARGS__); } }  



#endif