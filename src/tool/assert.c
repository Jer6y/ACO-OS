#include "Los_type.h"
#include "Los_err.h"
#include "bsp_port.h"
#include "arch_port.h"
#include "string.h"
//上面是依赖 [依赖哪些公共库和哪些模块]
//下面是实现 [实现了哪些公共库或者哪些模块]
#include "assert.h"

void panic(char *str,...)
{
    hwi_lock();
    va_list vlist;
    uint64 s0_addr;
    asm volatile("sd s0, %0":"=m"(s0_addr)::"memory");
    va_start(vlist,s0_addr);
    char tmp_buf[256];
    int i = vsprintf(tmp_buf,str,vlist);
    uartputstr_sync(tmp_buf,i);
    while(1)
    {
        asm volatile("wfi");
    }
}


void assert_fail(char *file_name, int line)
{
    printf("===========================\n");
    printf("file:%s\n",file_name);
    printf("line:%d\n",line);
    printf("============================\n");
    while(1)
    {
        asm volatile("wfi");
    }
}