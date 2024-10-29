#ifndef __RISCV_PRINTF_H
#define __RISCV_PRINTF_H

#include <aco/types.h>
#include <sbi/debug_console.h>
#include <aco/string.h>

FUNC_BUILTIN int riscv_putc(int byte)
{
	sbi_debug_console_write_byte(byte);
	return (byte);
}

/* implement the porting implmentation */
#define __arch_put_char		riscv_putc

#endif
