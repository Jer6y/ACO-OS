/*
 *  stdarg 的实现
 *  用于变长函数参数传递
 * 
 */
#ifndef STD_ARG_H
#define STD_ARG_H

typedef char* va_list;

#define va_start(vlist,addrs_arg)   ((vlist) = (va_list)addrs_arg)

#define va_arg(args,type)   (*(type*)((va_list*)(args=(va_list)((va_list *)(args)+1))-1))

#define va_end(vlist) ((vlist) = (va_list)0)


#endif