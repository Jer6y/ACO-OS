#ifndef __ASM_LINK_H
#define __ASM_LINK_H

#include <generated/autoconf.h>

#define __ARCH_KERNEL_OFFSET  	0xffffffd800000000
//#define __ARCH_KERNEL_OFFSET  	0x80200000
//FFFFFFD77E000000
#define __ARCH_PAGE_ALIGN 	(1 << CONFIG_PAGE_OFFSET)

#define __ARCH_DATA_ALIGN	16

#define __ARCH_CODE_ALIGN 	16

#ifdef  CONFIG_RISCV_INIT_STACKSIZE
#define RISCV_INIT_STKSIZE	CONFIG_RISCV_INIT_STACKSIZE
#else
#define RISCV_INIT_STKSIZE	__ARCH_PAGE_ALIGN * 2
#endif

#ifdef CONFIG_RISCV_INIT_HEAPSIZE
#define RISCV_INIT_HAPSIZE	CONFIG_RISCV_INIT_HEAPSIZE
#else
#define RISCV_INIT_HAPSIZE	__ARCH_PAGE_ALIGN * 100
#endif

#define SECTION_SYSSTACK	.initstack (NOLOAD) : 			\
				{					\
					. = ALIGN(__ARCH_PAGE_ALIGN);	\
					__sinitstack_section = .;	\
					. += RISCV_INIT_STKSIZE;	\
					__einitstack_section = .;	\
				}		

#define SECTION_INITHEAP 	.initheap (NOLOAD) :			\
				{					\
					. = ALIGN(__ARCH_PAGE_ALIGN);	\
					__sinitheap_section = .;	\
					. += RISCV_INIT_HAPSIZE;	\
					__einitheap_section = .;	\
				}


#define __ARCH_SECTIONS		SECTION_SYSSTACK			\
				SECTION_INITHEAP

#endif

