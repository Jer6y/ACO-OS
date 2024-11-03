#ifndef __ASM_LINK_H
#define __ASM_LINK_H

#include <generated/autoconf.h>

#define __ARCH_KERNEL_OFFSET  	0x0000003000000000
//#define __ARCH_KERNEL_OFFSET  	0x80200000

#define __ARCH_PAGE_ALIGN 	(1 << CONFIG_PAGE_OFFSET)

#define __ARCH_DATA_ALIGN	16

#define __ARCH_CODE_ALIGN 	16

#define RISCV_DTB_SIZE		(0x200000) //fixed dtb size 2M

#define SECTION_RVDTB		.rvdtb (NOLOAD) :			\
				{					\
					. = ALIGN(__ARCH_PAGE_ALIGN);	\
					__srvdtb_section = .;		\
					*(.rvdtb)			\
					*(.rvdtb.*)			\
					__ervdtb_section = .;		\
				}



#define __ARCH_SECTIONS	    SECTION_RVDTB

#define get_rv_dtb() 	   ({                                  \
                              extern char __srvdtb_section[];  \
                              __srvdtb_section;                \
                            })

#endif

