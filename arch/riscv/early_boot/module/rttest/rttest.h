#ifndef __RTTEST_H
#define __RTTEST_H

#include <aco/types.h>
#include <aco/linkage.h>
#include <stdarg.h>
#include <printf.h>

typedef int (*t_handler)(int* success, int* error);

typedef struct test_entry_s {
        const char*     describe;
        t_handler       rttest_func;
} test_entry_t;

#define rttest_printf riscv_printf

#endif
