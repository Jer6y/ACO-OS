#include "Los_type.h"
#include "Los_err.h"
#include "stdarg.h"
#include "string.h"
#include "uart.h"
#include "riscv.h"

//上面是依赖 [依赖哪些公共库和哪些模块]
//下面是实现 [实现了哪些公共库或者哪些模块]
#include "assert.h"

void panic(char *str,...)
{
    hwi_lock();
    va_list vlist;
    uint64 s0_addr;
    asm volatile("sd s0, %0":"=m"(s0_addr)::"memory");
    s0_addr += 8;
    va_start(vlist,s0_addr);
    char tmp_buf[256];
    int i = vsprintf(tmp_buf,str,vlist);
    uartputstr_sync(tmp_buf,i);
    while(1)
    {
        w_int_off_s();
        asm volatile("wfi");
    }
}


void assert_failed(char *file_name, int line)
{
    printk("===========================\n");
    printk("file:%s\n",file_name);
    printk("line:%d\n",line);
    printk("============================\n");
    while(1)
    {
        w_int_off_s();
        asm volatile("wfi");
    }
}