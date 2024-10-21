#include <aco/types.h>
#include <aco/linkage.h>
#include <mm/page.h>
#include <mm/memblock_alloc.h>
#include <aco/printf.h>

PRIVATE_VAR mmblock_hdr_t early_mmallocator;

void setup_mm()
{
	uintptr_t heap_start = (uintptr_t)get_section_start(initheap);
	uintptr_t heap_end = (uintptr_t)get_section_end(initheap);
	//ASSERT(heap_end - heap_start == CONFIG_RISCV_INIT_HEAPSIZE);
	int ret = memblock_allocator_init(&early_mmallocator, heap_start, CONFIG_RISCV_INIT_HEAPSIZE, PAGE_SIZE);
	printf("ret : %d\n",ret);
}

void setup_vm(uintptr_t pa_dtb, uintptr_t core_id)
{
	
}
