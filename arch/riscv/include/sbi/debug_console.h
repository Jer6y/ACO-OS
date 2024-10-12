#ifndef __SBI_DEBUG_CONSOLE_H
#define __SBI_DEBUG_CONSOLE_H

#include "sbi_internal.h"

#define  DEBUGCONSOLE_EID 	0x4442434E
#define  FID_CONSOLE_WRITE 	0
#define  FID_CONSOLE_READ	1
#define  FID_CONSOLE_WRITBYT	2

static inline struct sbiret sbi_debug_console_write(unsigned long num_bytes,\
		unsigned long base_addr_lo,\
		unsigned long base_addr_hi)
{
	struct sbiret ret;
	PUT_PARAMETER_3(num_bytes,base_addr_lo,base_addr_hi);
	SBI_CALL(DEBUGCONSOLE_EID,FID_CONSOLE_WRITE);
	GET_SBIRET(ret);
	return ret;
}

static inline struct sbiret sbi_debug_console_read(unsigned long num_bytes,\
		unsigned long base_addr_lo,\
		unsigned long base_addr_hi)
{
	struct sbiret ret;
        PUT_PARAMETER_3(num_bytes,base_addr_lo,base_addr_hi);
        SBI_CALL(DEBUGCONSOLE_EID,FID_CONSOLE_READ);
        GET_SBIRET(ret);
        return ret;

}

static inline struct sbiret sbi_debug_console_write_byte(unsigned char byte)
{
	struct sbiret ret;
	PUT_PARAMETER_1(byte);
	SBI_CALL(DEBUGCONSOLE_EID,FID_CONSOLE_WRITBYT);
	GET_SBIRET(ret);
	return ret;
}

#endif
