#ifndef __ACO_ASSEMBLY_H
#define __ACO_ASSEMBLY_H

#include <aco/linkage.h>

#ifdef __ASSEMBLY__

/* some toolchain use other character to mark a neline in asm code such as '`' */
#define ASM_NEWLINE			;

#define ASM_ALIGN(alignment)		.balign  	alignment
#define ASM_NOALIGN(alignment)		/* do nothing */

#define ASM_SECTION(name,flags)		.section 	name,#flags

#define ASM_GLOBAL(symbol)		.global		symbol
#define ASM_LOCAL(symbol)		/* do nothing*/
#define ASM_WEAK(symbol)		.weak		symbol

#define ASM_EXTERN(symbol)		.extern		symbol
#define SYMBOL_START(name,alignment,func_attribute,func_align)	ASM_NEWLINE\
					func_align(alignment)	ASM_NEWLINE\
					func_attribute(name)	ASM_NEWLINE\
					name:

// symbol_end ".set .L__sym_size_##name, .-name" is copied from linux , it doesn't matter that we do not write this
// maybe in linux , it will be used in debug to trace symbols
#define SYMBOL_END(name, symbol_type)	.type	name,#symbol_type 	 	ASM_NEWLINE\
					.set .L__sym_size_##name, .-name 	ASM_NEWLINE\
        				.size name, .L__sym_size_##name		ASM_NEWLINE\
					.previous

//private , not use it unless you really need it
#define __FUNCTION_START(func_name,segment,alignment,func_attribute,func_align) 	ASM_SECTION(segment,ax)		ASM_NEWLINE\
											SYMBOL_START(func_name,alignment,func_attribute,func_align)
//private , not use it unless you really needit
#define __FUNCTION_END(func_name)							SYMBOL_END(func_name,function)

//private , not use it unless you really need it
#define __DATA_START(var_name,segment,alignment,var_attribute,func_align)		ASM_SECTION(segment,aw)		ASM_NEWLINE\
											SYMBOL_START(var_name,alignment,var_attribute,func_align)
//private , not use it unless you really need it
#define __DATA_END(var_name)								SYMBOL_END(var_name,object)

/*
 *  Name   : DEFINE_FUNCTION_START
 *  Desc   : define a function name 'func' in asm code
 *  Param  :
 *           func: function name defined in asm code
 *           seg : segment that 'func' will be put in
 *           attr: attribute for the 'func'
 *                 ASM_GLOBAL : function is global
 *                 ASM_LOCAL  : function is local
 *                 ASM_WEAK   : function is weak, if there has other function and its name same as 'func'
 *                              the implement of 'func' will be replaced by the function that name sam as 'func'
 *   Usage  :
 * 	DEFINE_FUNCTION_START(_start,.text,ASM_GLOBAL)
 *	add sp,sp,zero
 *	call main
 *	DEFINE_FUNCTION_END(_start)
 */
#define DEFINE_FUNCTION_START(func,seg,attr)		__FUNCTION_START(func,seg,0,attr,ASM_NOALIGN)

/*
 *  Name   : DEFINE_FUNCTION_START_ALIGN
 *  Desc   : define a function name 'func' in asm code
 *  Param  :
 *           func: function name defined in asm code
 *           seg : segment that 'func' will be put in
 *           attr: attribute for the 'func'
 *                 ASM_GLOBAL : function is global
 *                 ASM_LOCAL  : function is local
 *                 ASM_WEAK   : function is weak, if there has other function and its name same as 'func'
 *                              the implement of 'func' will be replaced by the function that name sam as 'func'
 *           algn: alignment for the 'func'
 *   Usage :
 *      DEFINE_FUNCTION_START_ALIGN(_start,.text,ASM_GLOBAL,4096)
 *      add sp,sp,zero
 *      call main
 *      DEFINE_FUNCTION_END(_start)
 */
#define DEFINE_FUNCTION_START_ALIGN(func,seg,attr,algn)	__FUNCTION_START(func,seg,algn,attr,ASM_ALIGN)

/*
 *  Name   : DEFINE_FUNCTION_END
 *  Desc   : end of a function define
 *  Param  :
 *           func: function name defined in asm code
 *   Usage :
 *      DEFINE_FUNCTION_START_ALIGN(_start,.text,ASM_GLOBAL,4096)
 *      add sp,sp,zero
 *      call main
 *      DEFINE_FUNCTION_END(_start)
 */
#define DEFINE_FUNCTION_END(func)			__FUNCTION_END(func)

/*
 *  Name   : DEFINE_DATA_START
 *  Desc   : define a variable name 'var' in asm code
 *  Param  :
 *           var : variable name defined in asm code
 *           seg : segment that 'var' will be put in
 *           attr: attribute for the 'var'
 *                 ASM_GLOBAL : var is global
 *                 ASM_LOCAL  : var is local
 *                 ASM_WEAK   : var is weak, if there has other variable and its name same as 'var'
 *                              the implement of 'var' will be replaced by the variable that name same as 'var'
 *   Usage :
 *      DEFINE_DATA_START(test_c,.data,ASM_GLOBAL)
 *      .byte  1,2,3
 *      .ascii 'a','b'
 *      DEFINE_DATA_END(test_c)
 */
#define DEFINE_DATA_START(var,seg,attr)			__DATA_START(var,seg,0,attr,ASM_NOALIGN)

/*
 *  Name   : DEFINE_DATA_START_ALIGN
 *  Desc   : define a variable name 'var' in asm code
 *  Param  :
 *           var : variable name defined in asm code
 *           seg : segment that 'var' will be put in
 *           attr: attribute for the 'var'
 *                 ASM_GLOBAL : var is global
 *                 ASM_LOCAL  : var is local
 *                 ASM_WEAK   : var is weak, if there has other variable and its name same as 'var'
 *                              the implement of 'var' will be replaced by the variable that name same as 'var'
 *           algn: alignment for the 'var'
 *   Usage :
 *      DEFINE_DATA_START_ALIGN(test_b,.data,ASM_GLOBAL,4096)
 *      .byte  1,2,3
 *      .ascii 'a','b'
 *      DEFINE_DATA_END(test_b)
 */
#define DEFINE_DATA_START_ALIGN(var,seg,attr,algn)	__DATA_START(var,seg,algn,attr,ASM_ALIGN)

/*
 *  Name   : DEFINE_DATA_END
 *  Desc   : end of DEFINE_DATA_START/DEFINE_DATA_START_ALIGN
 *  Param  :
 *           var : variable name defined in asm code
 *   Usage :
 *      DEFINE_DATA_START_ALIGN(test_a,.data,ASM_GLOBAL,4096)
 *      .byte  1,2,3
 *      .ascii 'a','b'
 *      DEFINE_DATA_END(test_a)
 */
#define DEFINE_DATA_END(var)				__DATA_END(var)

/*
 *  Name   : HEAD_DATA
 *  Desc   : define a variable named 'var_name' in the .head section
 *  Param  :
 *           var_name : variable name defined in asm code
 *   Usage :
 *      HEAD_DATA(head_data_test)
 *      .byte  1,2,3
 *      .ascii 'a','b'
 *      HEAD_DATA_END(head_data_test)
 */
#define HEAD_DATA(var_name)				__DATA_START(var_name,.head.data,0,ASM_GLOBAL,ASM_NOALIGN)

/*
 *  Name   : HEAD_DATA_ALIGN
 *  Desc   : define a variable named 'var_name' in the .head section
 *  Param  :
 *           var_name : variable name defined in asm code
 *           alignment : align for the 'var'
 *   Usage :
 *      HEAD_DATA_ALIGN(head_data_test,4096)
 *      .byte  1,2,3
 *      .ascii 'a','b'
 *      HEAD_DATA_END(head_data_test)
 */
#define HEAD_DATA_ALIGN(var_name,alignment)		__DATA_START(var_name,.head.data,alignment,ASM_GLOBAL,ASM_ALIGN)

/*
 *  Name   : HEAD_DATA_END
 *  Desc   : end of a 'HEAD_DATA' or 'HEAD_DATA_ALIGN'
 *  Param  :
 *           var_name : variable name defined in asm code
 *   Usage :
 *      HEAD_DATA_ALIGN(head_data_test,4096)
 *      .byte  1,2,3
 *      .ascii 'a','b'
 *      HEAD_DATA_END(head_data_test)
 */
#define HEAD_DATA_END(var_name)				__DATA_END(var_name)

/*
 *  Name   : HEAD_DATA_PRIVATE
 *  Desc   : define a variable named 'var_name' in the .head section and it is a local variable
 *  Param  :
 *           var_name : variable name defined in asm code
 *   Usage :
 *      HEAD_DATA_PRIVATE(head_data_test)
 *      .byte  1,2,3
 *      .ascii 'a','b'
 *      HEAD_DATA_PRIVATE_END(head_data_test)
 */
#define HEAD_DATA_PRIVATE(var_name)			__DATA_START(var_name,.head.data,0,ASM_LOCAL,ASM_NOALIGN)

/*
 *  Name   : HEAD_DATA_PRIVATE_AILGN
 *  Desc   : define a variable named 'var_name' in the .head section and it is a local variable
 *  Param  :
 *           var_name : variable name defined in asm code
 *           alignment : align for the 'var_name'
 *   Usage :
 *      HEAD_DATA_PRIVATE_ALIGN(head_data_test,4096)
 *      .byte  1,2,3
 *      .ascii 'a','b'
 *      HEAD_DATA_PRIVATE_END(head_data_test)
 */
#define HEAD_DATA_PRIVATE_ALIGN(var_name,alignment)	__DATA_START(var_name,.head.data,alignment,ASM_LOCAL,ASM_ALIGN)

/*
 *  Name   : HEAD_DATA_PRIVATE_END
 *  Desc   : end define for 'HEAD_DATA_PRIVATE_ALIGN' or 'HEAD_DATA_PRIVATE'
 *  Param  :
 *           var_name : variable name defined in asm code
 *   Usage :
 *      HEAD_DATA_PRIVATE_ALIGN(head_data_test,4096)
 *      .byte  1,2,3
 *      .ascii 'a','b'
 *      HEAD_DATA_PRIVATE_END(head_data_test)
 */
#define HEAD_DATA_PRIVATE_END(var_name)			__DATA_END(var_name)

/*
 *  Name   : HEAD_DATA_WEAK
 *  Desc   : define a variable named 'var_name' in the .head section and it is a weak variable
 *  Param  :
 *           var_name : variable name defined in asm code
 *   Usage :
 *      HEAD_DATA_WEAK(head_data_test)
 *      .byte  1,2,3
 *      .ascii 'a','b'
 *      HEAD_DATA_WEAK_END(head_data_test)
 */
#define HEAD_DATA_WEAK(var_name)			__DATA_START(var_name,.head.data,0,ASM_WEAK,ASM_NOALIGN)

/*
 *  Name   : HEAD_DATA_WEAK_AILGN
 *  Desc   : define a variable named 'var_name' in the .head section and it is a weak variable
 *  Param  :
 *           var_name : variable name defined in asm code
 *           alignment : align for the 'var_name'
 *   Usage :
 *      HEAD_DATA_WEAK_ALIGN(head_data_test,4096)
 *      .byte  1,2,3
 *      .ascii 'a','b'
 *      HEAD_DATA_WEAK_END(head_data_test)
 */
#define HEAD_DATA_WEAK_ALIGN(var_name,alignment)	__DATA_START(var_name,.head.data,alignment,ASM_WEAK,ASM_ALIGN)

/*
 *  Name   : HEAD_DATA_WEAK_END
 *  Desc   : end define for 'HEAD_DATA_WEAK_ALIGN' or 'HEAD_DATA_WEAK'
 *  Param  :
 *           var_name : variable name defined in asm code
 *   Usage :
 *      HEAD_DATA_WEAK_ALIGN(head_data_test,4096)
 *      .byte  1,2,3
 *      .ascii 'a','b'
 *      HEAD_DATA_WEAK_END(head_data_test)
 */
#define HEAD_DATA_WEAK_END(var_name)			__DATA_END(var_name)

#define HEADFUNC_START_ALIGN(func_name,alignment)	__FUNCTION_START(func_name,.head.text,alignment,ASM_GLOBAL,ASM_ALIGN)
#define HEADFUNC_START(func_name)			__FUNCTION_START(func_name,.head.text,0,ASM_GLOBAL,ASM_NOALIGN)
#define HEADFUNC_END(func_name)				__FUNCTION_END(func_name)

#define HEADFUNC_START_PRIVATE_ALIGN(func_name,alignment)	__FUNCTION_START(func_name,.head.text,alignment,ASM_LOCAL,ASM_ALIGN)
#define HEADFUNC_START_PRIVATE(func_name)			__FUNCTION_START(func_name,.head.text,0,ASM_LOCAL,ASM_NOALIGN)
#define HEADFUNC_END_PRIVATE(func_name)				__FUNCTION_END(func_name)

#define HEADFUNC_START_WEAK_ALIGN(func_name,alignment)		__FUNCTION_START(func_name,.head.text,alignment,ASM_WEAK,ASM_ALIGN)
#define HEADFUNC_START_WEAK(func_name)				__FUNCTION_START(func_name,.head.text,0,ASM_WEAK,ASM_NOALIGN)
#define HEADFUNC_END_WEAK(func_name)				__FUNCTION_END(func_name)

#define VALDATA(var_name)                             __DATA_START(var_name,.data,0,ASM_GLOBAL,ASM_NOALIGN)
#define VALDATA_ALIGN(var_name,alignment)             __DATA_START(var_name,.data,alignment,ASM_GLOBAL,ASM_ALIGN)
#define VALDATA_END(var_name)                         __DATA_END(var_name)

#define VALDATA_PRIVATE(var_name)                     __DATA_START(var_name,.data,0,ASM_LOCAL,ASM_NOALIGN)
#define VALDATA_PRIVATE_ALIGN(var_name,alignment)     __DATA_START(var_name,.data,alignment,ASM_LOCAL,ASM_ALIGN)
#define VALDATA_PRIVATE_END(var_name)                 __DATA_END(var_name)

#define VALDATA_WEAK(var_name)                        __DATA_START(var_name,.data,0,ASM_WEAK,ASM_NOALIGN)
#define VALDATA_WEAK_ALIGN(var_name,alignment)        __DATA_START(var_name,.data,alignment,ASM_WEAK,ASM_ALIGN)
#define VALDATA_WEAK_END(var_name)                    __DATA_END(var_name)

#define FUNC_START_ALIGN(func_name,alignment)		__FUNCTION_START(func_name,.text,alignment,ASM_GLOBAL,ASM_ALIGN)
#define FUNC_START(func_name)				__FUNCTION_START(func_name,.text,0,ASM_GLOBAL,ASM_NOALIGN)
#define FUNC_END(func_name)				__FUNCTION_END(func_name)

#define FUNC_START_PRIVATE_ALIGN(func_name,alignment)	__FUNCTION_START(func_name,.text,alignment,ASM_LOCAL,ASM_ALIGN)
#define FUNC_START_PRIVATE(func_name)			__FUNCTION_START(func_name,.text,0,ASM_LOCAL,ASM_NOALIGN)
#define FUNC_END_PRIVATE(func_name)			__FUNCTION_END(func_name)

#define FUNC_START_WEAK_ALIGN(func_name,alignment)	__FUNCTION_START(func_name,.text,alignment,ASM_WEAK,ASM_ALIGN)
#define FUNC_START_WEAK(func_name)			__FUNCTION_START(func_name,.text,0,ASM_WEAK,ASM_NOALIGN)
#define FUNC_END_WEAK(func_name)			__FUNCTION_END(func_name)

#define BSSDATA(var_name)                             __DATA_START(var_name,.bss,0,ASM_GLOBAL,ASM_NOALIGN)
#define BSSDATA_ALIGN(var_name,alignment)             __DATA_START(var_name,.bss,alignment,ASM_GLOBAL,ASM_ALIGN)
#define BSSDATA_END(var_name)                         __DATA_END(var_name)

#define BSSDATA_PRIVATE(var_name)                     __DATA_START(var_name,.bss,0,ASM_LOCAL,ASM_NOALIGN)
#define BSSDATA_PRIVATE_ALIGN(var_name,alignment)     __DATA_START(var_name,.bss,alignment,ASM_LOCAL,ASM_ALIGN)
#define BSSDATA_PRIVATE_END(var_name)                 __DATA_END(var_name)

#define BSSDATA_WEAK(var_name)                        __DATA_START(var_name,.bss,0,ASM_WEAK,ASM_NOALIGN)
#define BSSDATA_WEAK_ALIGN(var_name,alignment)        __DATA_START(var_name,.bss,alignment,ASM_WEAK,ASM_ALIGN)
#define BSSDATA_WEAK_END(var_name)                    __DATA_END(var_name)


#endif /* __ASSEMBLY__ */

#endif
