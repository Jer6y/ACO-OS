#include <aco/types.h>
#include <aco/linkage.h>
#include <mm/memblock_alloc.h>
#include <aco/init.h>

void setup_mm()
{
	uintptr_t heap_start = (uintptr_t)get_section_start(initheap);
	uintptr_t heap_end = (uintptr_t)get_section_end(initheap);
}

void setup_vm(uintptr_t pa_dtb, uintptr_t core_id)
{
	
}
