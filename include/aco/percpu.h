#ifndef __ACO_PERCPU_H
#define __ACO_PERCPU_H

#include <aco/linkage.h>
#include <aco/types.h>

struct percpu_var {
	uintptr_t  ptr_percpu;
	uintptr_t  size_percpu;
} PACKED;
get_section_end
#define PER_CPU(var_type, var_name)	USED PUTINTO_SEGMENT(percpu) struct percpu_var percpu_##varname = { 0, sizeof(var_type) };
static inline int per_cpu_init()
{
	uintptr_t per_cpu_start = get_section_start(percpu);
	uintptr_t per_cpu_end   = get_section_end(percpu);
	
}


#endif /* __ACO_PERCPU_H */
