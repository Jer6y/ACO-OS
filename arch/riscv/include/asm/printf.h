#ifndef __RISCV_PRINTF_H
#define __RISCV_PRINTF_H

#include <aco/types.h>
#include <stdarg.h>
#include <sbi/debug_console.h>
#include <asm/string.h>

FUNC_BUILTIN int riscv_putc(char byte)
{
	sbi_debug_console_write_byte(byte);
	return 0;
}

FUNC_BUILTIN int riscv_puts(const char* str)
{
	int size = strlen(str);
	uint64 address = (uint64)str;
	sbi_debug_console_write(size, address, 0);
        return 0;
}


FUNC_BUILTIN int riscv_printf(const char* fmt, ...)
{
	char buffer[256];
  	va_list arg;
        va_start(arg, fmt);
        int done = vsprintf(buffer, fmt, arg);  // 将格式化的内容(字符串)保存在buffer中
  	riscv_puts(buffer);
  	va_end(arg);
  	return done;
}






#endif
