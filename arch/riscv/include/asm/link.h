#ifndef __ASM_LINK_H
#define __ASM_LINK_H

#include <generated/autoconf.h>

#define __ARCH_KERNEL_OFFSET  	0x0000003000000000
//#define __ARCH_KERNEL_OFFSET  	0x80200000

#define __ARCH_PAGE_ALIGN 	(1 << CONFIG_PAGE_OFFSET)

#define __ARCH_DATA_ALIGN	16

#define __ARCH_CODE_ALIGN 	16

#ifdef  CONFIG_RISCV_INIT_STACKSIZE
#define RISCV_INIT_STKSIZE	CONFIG_RISCV_INIT_STACKSIZE
#else
#define RISCV_INIT_STKSIZE	__ARCH_PAGE_ALIGN * 2
#endif

#define RISCV_DTB_SIZE		(0x200000) //fixed dtb size 2M

#define SECTION_SYSSTACK	.initstack (NOLOAD) : 			\
				{					\
					. = ALIGN(__ARCH_PAGE_ALIGN);	\
					__sinitstack_section = .;	\
					. += RISCV_INIT_STKSIZE;	\
					__einitstack_section = .;	\
				}		
#define SECTION_RVDTB		.rvdtb (NOLOAD) :			\
				{					\
					. = ALIGN(__ARCH_PAGE_ALIGN);	\
					__srvdtb_section = .;		\
					*(.rvdtb)			\
					*(.rvdtb.*)			\
					__ervdtb_section = .;		\
				}



#define __ARCH_SECTIONS		SECTION_RVDTB				\
				SECTION_SYSSTACK			

#define get_seperate_line() ({                                  \
                              extern char _seperate_line[];     \
                              _seperate_line;                   \
                            })

#define get_rv_dtb() 	   ({                                  \
                              extern char __srvdtb_section[];  \
                              __srvdtb_section;                \
                            })

#endif

