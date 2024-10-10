#ifndef __ACO_LINK_H
#define __ACO_LINK_H


#define DEFINE_LKSCRIPT_SEGMENT(segname, align)		\
				. = ALIGN(align);	\
				__s##segname = .;	\
				*(.segname)		\
				*(.segname.*)		\
				__e##segname = .;


#define  SDATA(align)	DEFINE_LKSCRIPT_SEGMENT(sdata,align)

#define  DATA(align)	DEFINE_LKSCRIPT_SEGMENT(data,align)

#define  RODATA(align)	DEFINE_LKSCRIPT_SEGMENT(rodata,align)

#define  SRODATA(align)	DEFINE_LKSCRIPT_SEGMENT(srodata,align)

#define  CODTEXT(align)	DEFINE_LKSCRIPT_SEGMENT(text,align)

#define  SBSS(align)	DEFINE_LKSCRIPT_SEGMENT(sbss,align)

#define  BSS(align)	DEFINE_LKSCRIPT_SEGMENT(bss,align)

#define  RTMTEST(align) DEFINE_LKSCRIPT_SEGMENT(rttest,align)




#endif
