#ifndef __RISCV_PRINTF_H
#define __RISCV_PRINTF_H

#include <aco/types.h>
#include <stdarg.h>
#include <sbi/debug_console.h>
#include <aco/string.h>

FUNC_BUILTIN int riscv_putc(int byte)
{
	sbi_debug_console_write_byte(byte);
	return (byte);
}

FUNC_BUILTIN int riscv_puts(const char* str)
{
	int size = strlen(str);
	for(int i=0;i<size;i++)
		riscv_putc(str[i]);
	riscv_putc('\n');
        return size;
}


FUNC_BUILTIN int riscv_printf(const char* fmt, ...)
{
	char buffer[512];
  	va_list arg;
        va_start(arg, fmt);
        int done = vsnprintf(buffer, 512, fmt, arg);  // 将格式化的内容(字符串)保存在buffer中
  	va_end(arg);
	int size = strlen(buffer);
        for(int i=0;i<size;i++)
		riscv_putc(buffer[i]);
  	return done;
}

#include <csr.h>
FUNC_BUILTIN void riscv_assertion(const char* error_condition)
{
	intr_off(); //disable local interrupt
	while(1)
		asm volatile("wfi");
	//just spinself
}

/* implement the porting implmentation */
#define __ARCH_PRINTF 		riscv_printf
#define __ARCH_PUTS		riscv_puts
#define __ARCH_PUTCHAR		riscv_putc
#define __ARCH_ASSERT_ERROR(x)	riscv_assertion(#x)


#endif
