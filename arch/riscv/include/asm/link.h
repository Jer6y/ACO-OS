#ifndef __ASM_LINK_H
#define __ASM_LINK_H

#include <generated/autoconf.h>

#define __ARCH_KERNEL_OFFSET  	0xffffffd800000000

#define __ARCH_PAGE_ALIGN 	4096

#define __ARCH_DATA_ALIGN	16

#define __ARCH_CODE_ALIGN 	16

#ifdef  CONFIG_RISCV_INIT_STACKSIZE
#define RISCV_INIT_STKSIZE	CONFIG_RISCV_INIT_STACKSIZE
#else
#define RISCV_INIT_STKSIZE	__ARCH_PAGE_ALIGN * 2
#endif

#define SECTION_SYSSTACK	.initstack (NOLOAD) : 			\
				{					\
					. = ALIGN(__ARCH_PAGE_ALIGN);	\
					__sinitstack_section = .;	\
					. += RISCV_INIT_STKSIZE;	\
					__einitstack_section = .;	\
				}					

#define __ARCH_SECTIONS		SECTION_SYSSTACK

#endif

