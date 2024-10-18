#ifndef __ACO_INIT_H
#define __ACO_INIT_H

#include <aco/linkage.h>
#include <aco/types.h>
#include <generated/autoconf.h>
typedef int  (*init_func)(void);
typedef void (*exit_func)(void);

#ifdef CONFIG_RTTEST

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


#else  /* NOT CONFIG_RTTEST */

FUNC_BUILTIN int aco_module_init(void)
{
	return 0;
}

FUNC_BUILTIN int aco_module_exit(void)
{
        return 0;
}

#endif /* CONFIG_RTTEST END */


#define VALID_MODULE_PRIO(prio) 	  (prio >=0 && prio <=7)
#define PRIO_0				  0
#define PRIO_1				  1
#define PRIO_2				  2
#define PRIO_3				  3
#define PRIO_4				  4
#define PRIO_5				  5
#define PRIO_6				  6
#define PRIO_7				  7
#define PRIO_LOWEST			  PRIO_7
#define PRIO_HIGHEST			  PRIO_0
#define REGISTER_MODULE_INIT(prio, func)  PRIVATE_VAR PUTINTO_INIT(prio) init_func init_function_##func = func;
#define REGISTER_MODULE_EXIT(prio, func) 


#endif
