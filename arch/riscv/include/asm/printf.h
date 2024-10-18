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
	uint64 address = (uint64)str;
	sbi_debug_console_write(size, address, 0);
        return size;
}


FUNC_BUILTIN int riscv_printf(const char* fmt, ...)
{
	char buffer[512];
  	va_list arg;
        va_start(arg, fmt);
        int done = vsnprintf(buffer, 512, fmt, arg);  // 将格式化的内容(字符串)保存在buffer中
  	riscv_puts(buffer);
  	va_end(arg);
  	return done;
}

#define __ARCH_PRINTF 	riscv_printf
#define __ARCH_PUTS	riscv_puts
#define __ARCH_PUTCHAR	riscv_putc


#endif
