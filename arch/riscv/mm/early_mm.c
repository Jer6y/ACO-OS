#include <mm/early_mm.h>
#include <aco/types.h>
#include <mm/page.h>
#include <aco/linkage.h>
#include <aco/string.h>
#include <aco/printf.h>

PRIVATE_VAR mmblock_hdr_t early_mmallocator;
PRIVATE_VAR int is_init = 0;

static int fresh_address(block_hdr_t* blk)
{
	struct list_head* ptr = &(blk->_node);
	ptr->prev = (struct list_head*)((uintptr_t)(ptr->prev) + RISCV_VA2PA_OFFSET);
	ptr->next = (struct list_head*)((uintptr_t)(ptr->next) + RISCV_VA2PA_OFFSET);
	return 0;
}

int   refresh_for_mmu()
{
	int ret;
	ASSERT(is_init == 1);
	ret = memblock_for_each_safe(&early_mmallocator, fresh_address);
	ASSERT(ret == 0);
	early_mmallocator._list_block.prev = (struct list_head*)((uintptr_t)(early_mmallocator._list_block.prev) + RISCV_VA2PA_OFFSET);
	early_mmallocator._list_block.next = (struct list_head*)((uintptr_t)(early_mmallocator._list_block.next) + RISCV_VA2PA_OFFSET);
	return ret;
}

static int mem_test_echo(block_hdr_t* blk)
{
	printf("address %p\n",blk);
	return 0;
}
void  mem_test(void)
{
	memblock_for_each_safe(&early_mmallocator, mem_test_echo);
	return;
}

int setup_earlymm()
{
	int ret;
        uintptr_t heap_start = (uintptr_t)get_section_start(initheap);
        uintptr_t heap_end = (uintptr_t)get_section_end(initheap);
        ASSERT(heap_end - heap_start == CONFIG_RISCV_INIT_HEAPSIZE);
	is_init = 1;
	ret = memblock_allocator_init(&early_mmallocator, heap_start, CONFIG_RISCV_INIT_HEAPSIZE, PAGE_SIZE);
	ASSERT(ret == 0);
        return ret;
}

void* mem_alloc()
{
	ASSERT(is_init == 1);
	void* ptr = memblock_alloc(&early_mmallocator);
	ASSERT(ptr != NULL);
	memset(ptr,0,PAGE_SIZE);
	return ptr;
}

void  mem_free(void* address)
{
	ASSERT(is_init == 1);
	memblock_free(&early_mmallocator, address);
}
