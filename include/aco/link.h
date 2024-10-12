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

#define  INIT(align)	. = ALIGN(align);	\
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

#define  PUTINTO_INIT(prio) 	__attribute__((used)) __attribute__((section(".initcall"#prio".init")))

#define get_segment_start(seg) ({ 			\
    				char* __result; 	\
    				extern char __s##seg[]; \
    				__result = __s##seg;	\
   	 			__result; 		\
				})

#define get_segment_end(seg) ({ 			\
    				char* __result; 	\
    				extern char __e##seg[]; \
    				__result = __e##seg;	\
   	 			__result; 		\
				})


#endif
