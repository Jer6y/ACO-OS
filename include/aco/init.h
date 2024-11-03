#ifndef __ACO_INIT_H
#define __ACO_INIT_H

#include <aco/linkage.h>
#include <aco/types.h>
#include <generated/autoconf.h>
typedef int  (*init_func)(void);
typedef void (*exit_func)(void);

FUNC_BUILTIN int aco_module_init(void)
{

	char* init_start = get_segment_start(init);
	char* init_end   = get_segment_end(init);
	for(; init_start < init_end; init_start += sizeof(void*))
	{
		init_func ptr_func = *((init_func*)init_start);
		int error = ptr_func();
		if(error != 0)
			return error;
	}
	return 0;
}

FUNC_BUILTIN int aco_module_exit(void)
{
	return 0;
}

#define VALID_MODULE_PRIO(prio) 	  ((int)(prio) >=0 && (int)(prio) <32)
#define PRIO_0				  0
#define PRIO_1				  1
#define PRIO_2				  2
#define PRIO_3				  3
#define PRIO_4				  4
#define PRIO_5				  5
#define PRIO_6				  6
#define PRIO_7				  7
#define PRIO_8				  8
#define PRIO_9				  9
#define PRIO_10				  10
#define PRIO_11				  11
#define PRIO_12				  12
#define PRIO_13				  13
#define PRIO_14				  14
#define PRIO_15				  15
#define PRIO_16				  16
#define PRIO_17				  17
#define PRIO_18				  18
#define PRIO_19				  19
#define PRIO_20				  20
#define PRIO_21				  21
#define PRIO_22				  22
#define PRIO_23				  23
#define PRIO_24				  24
#define PRIO_25				  25
#define PRIO_26				  26
#define PRIO_27				  27
#define PRIO_28				  28
#define PRIO_29				  29
#define PRIO_30				  30
#define PRIO_31				  31
#define REGISTER_MODULE_INIT(prio, func)  PRIVATE_VAR PUTINTO_INIT(prio) init_func init_function_##func = func;
#define REGISTER_MODULE_EXIT(prio, func) 


#endif
