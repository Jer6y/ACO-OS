#ifndef __ACO_LINK_H
#define __ACO_LINK_H

// this file is used in linker scripts
// do not use this directly in c/asm code
// you can include linkage.h in c/asm code
// to do something related to segment that linker scripts need
/* a classic linker script might be the format of this: 
 * OUTPUT_ARCH(...)
 * ENTRY(...)
 * #include <aco/link.h>
 * SECTIONS
 *  {
 *    . = KERNEL_OFFSET;
 *    LINK_ALIGN(PAGE_ALIGN)
 *    KERNEL_START
 *    SECTION_HEAD
 *
 *    SECTION_TEXT
 *
 *    SECTION_DATA
 *
 *    SECTION_PERCPU
 *
 *    SECTION_INIT
 *
 *    SECTION_BSS
 *
 *    SECTIONS_ARCH_SPECIFIC
 *    KERNEL_END
 *  }
*/


// define a segment that contains ".segname" segment and all sub segment ".segname.*"
#define DEFINE_LKSCRIPT_SEGMENT_ALL(segname, align)	\
				   . = ALIGN(align);	\
				   __s##segname = .;	\
				   *(.segname)		\
				   *(.segname.*)	\
				   __e##segname = .;

// define a segment that contains ".segnam" segment
#define DEFINE_LKSCRIPT_SEGMENT(segname, align)		\
				   . = ALIGN(align);	\
				   __s##segname = .;	\
				   *(.segname)		\
				   __e##segname = .;

/* remember, if head text want to use global varible or use address to locate code or data
 * must through a translate layer */
#define  HEADTEXT(align) 	DEFINE_LKSCRIPT_SEGMENT_ALL(head,align)
#define  HEADTEXT_NOALIGN	HEADTEXT(1)

#define  SDATA(align)	 	DEFINE_LKSCRIPT_SEGMENT_ALL(sdata,align)
#define  SDATA_NOALIGN		SDATA(1)

#define  DATA(align)	 	DEFINE_LKSCRIPT_SEGMENT_ALL(data,align)
#define  DATA_NOALIGN		DATA(1)

#define  RODATA(align)	 	DEFINE_LKSCRIPT_SEGMENT_ALL(rodata,align)
#define  RODATA_NOALIGN		RODATA(1)

#define  SRODATA(align)	 	DEFINE_LKSCRIPT_SEGMENT_ALL(srodata,align)
#define  SRODATA_NOALIGN	SRODATA(1)

#define  CODTEXT(align)	 	DEFINE_LKSCRIPT_SEGMENT_ALL(text,align)
#define  CODTEXT_NOALIGN	CODTEXT(1)

#define  SBSS(align)	 	DEFINE_LKSCRIPT_SEGMENT_ALL(sbss,align)
#define  SBSS_NOALIGN		SBSS(1)

#define  BSS(align)	 	DEFINE_LKSCRIPT_SEGMENT_ALL(bss,align)
#define  BSS_NOALIGN		BSS(1)

#define  PERCPU(align)		DEFINE_LKSCRIPT_SEGMENT_ALL(percpu,align)
#define  PERCPU_NOALIGN		PERCPU(1)

#define  INIT(align)	 	. = ALIGN(align);	\
				__sinit = .;		\
			 	*(.initcall0.init)	\
			 	*(.initcall1.init)	\
			 	*(.initcall2.init)	\
			 	*(.initcall3.init)	\
				*(.initcall4.init)	\
			 	*(.initcall5.init)	\
			 	*(.initcall6.init)	\
			 	*(.initcall7.init)	\
			 	__einit = .;
#define  INIT_NOALIGN		INIT(1)

#define  LINK_ALIGN(align)	. = ALIGN(align);

#define  KERNEL_START		_start_kernel = .;
#define  KERNEL_END		_end_kernel = .;

/* include arch specific implement */
#include <asm/link.h>

/* arch specific implement for kernel_offset 
 * KERNEL_OFFSET is used in ld scripts (also can use in c code)
 * KERNEL_OFFSET is kernel start address , all code will start from here
 */
#define  KERNEL_OFFSET  __ARCH_KERNEL_OFFSET
#ifndef  KERNEL_OFFSET
#define  KERNEL_OFFSET 0
#endif

#define  PAGE_ALIGN	__ARCH_PAGE_ALIGN
#ifndef  PAGE_ALIGN
#define  PAGE_ALIGN 4096
#endif

#define  DATA_ALIGN	__ARCH_DATA_ALIGN
#ifndef  DATA_ALIGN
#define  DATA_ALIGN 8
#endif

#define  CODE_ALIGN	__ARCH_CODE_ALIGN
#ifndef  CODE_ALIGN
#define  CODE_ALIGN 8
#endif

#define  SECTIONS_ARCH_SPECIFIC  __ARCH_SECTIONS
#ifndef  SECTIONS_ARCH_SPECIFIC
#define  SECTIONS_ARCH_SPECIFIC
#endif

#define  SECTION_HEAD           .head :                 	\
                                {                       	\
					__shead_section = .;	\
                                        HEADTEXT_NOALIGN    	\
					__ehead_section = .;	\
                                }

#define  SECTION_TEXT           .text :                 	\
                                {                      	 	\
					__stext_section = .;	\
                                        CODTEXT(CODE_ALIGN)     \
					SRODATA(DATA_ALIGN)	\
					RODATA(DATA_ALIGN)	\
					__etext_section = .;	\
                                }

#define  SECTION_DATA		.data :				\
				{				\
					__sdata_section = .;	\
					SDATA(DATA_ALIGN)	\
					DATA(DATA_ALIGN)	\
					__edata_section = .;	\
				}

#define  SECTION_BSS		.bss (NOLOAD) :			\
				{				\
					__sbss_section = .;	\
					SBSS(DATA_ALIGN)	\
					BSS(DATA_ALIGN)		\
					__ebss_section = .;	\
				}

#define  SECTION_INIT		.init : 			\
				{				\
					__sinit_section = .;	\
					INIT(DATA_ALIGN)	\
					__einit_section	= .;	\
				}

//for percpu, all data in it is
//pointer!
#define  SECTION_PERCPU		.percpu : 			\
				{				\
					__spercpu_section = .;	\
					PERCPU(DATA_ALIGN)	\
					__epercpu_section = .;	\
				}

#endif
