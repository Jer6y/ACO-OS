#ifndef LOS_TYPE_H
#define LOS_TYPE_H


#define true    1
#define false   0
#define NULL    0

typedef char                int8;
typedef short               int16;
typedef int                 int32;
typedef long long           int64;
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned int        size_t;
typedef unsigned long long  uint64;
typedef uint8               bool;


typedef enum LOS_TYPE_E {
    LOS_TICK_LIST = 7842u   ,
    LOS_TICK_NODE           ,

    LOS_PROC                ,
    LOS_PROC_ALLOCATOR      ,

    LOS_RDY_LIST            ,
    LOS_RDY_QUEUE           ,

    LOS_TIMER               ,

    LOS_MUTEX               ,

    LOS_SEM                 ,

} los_type_t;


#endif