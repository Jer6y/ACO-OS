#ifndef __ACO_MMBLOCK_H
#define __ACO_MMBLOCK_H

#include <aco/types.h>
#include <aco/list.h>
#include <generated/autoconf.h>

//memory block allocator handler struct
//no lock for it
typedef struct mmblock_hdr_s {
	uint16		magic_number	;
	uintptr_t 	pa_start	;
	uint32	  	block_size	;
	uint32	  	total_block	;
	uint32 	  	rest_block	;
	struct list_head _list_block	; //_list_block hang the block_hdr node
} mmblock_hdr_t;

#define MAGIC_NUMBER 	0x2371
//one block handler, this is site in the
//the start address of the block want allocated
typedef struct block_hdr_s {
	uint16		  magic_number	;
	struct list_head  _node		;
} block_hdr_t;

typedef int (*foreach_func)(block_hdr_t* blk);
typedef int (*output_func)(const char* fmt,...);

#ifdef CONFIG_MEM_BLOCK_ALOCTOR

int   memblock_allocator_init(mmblock_hdr_t* mem_aloc_hdr, uint64 pa, uint64 size, uint32 block_size);

void* memblock_alloc(mmblock_hdr_t* mem_aloc_hdr);

void  memblock_free(mmblock_hdr_t* mem_aloc_hdr, void* address);

int   memblock_for_each(mmblock_hdr_t* mem_aloc_hdr, foreach_func iterate_func);
int   memblock_for_each_safe(mmblock_hdr_t* mem_aloc_hdr, foreach_func iterate_func);

int   memblock_allocator_merge(mmblock_hdr_t* mem_allocator, mmblock_hdr_t* merged_mem_allocator);

int   memblock_self_check(mmblock_hdr_t* mem_aloc_hdr);

int   memblock_self_check_with_output(mmblock_hdr_t* mem_aloc_hdr,output_func output);

#else /* NOT CONFIG CONFIG_MEM_BLOCK_ALOCTOR*/

static inline  int   memblock_allocator_init(mmblock_hdr_t* mem_aloc_hdr, uint64 pa, uint64 size, uint32 block_size)
{
	return 0;
}

static inline void* memblock_alloc(mmblock_hdr_t* mem_aloc_hdr)
{
	return NULL;
}

static inline void  memblock_free(mmblock_hdr_t* mem_aloc_hdr, void* address)
{
	return;
}

static inline int   memblock_for_each(mmblock_hdr_t* mem_aloc_hdr, foreach_func iterate_func)
{
	return 0;
}

static inline int   memblock_for_each_safe(mmblock_hdr_t* mem_aloc_hdr, foreach_func iterate_func)
{
	return 0;
}

static inline int   memblock_allocator_merge(mmblock_hdr_t* mem_allocator, mmblock_hdr_t* merged_mem_allocator)
{
	return 0;
}

static inline int   memblock_self_check(mmblock_hdr_t* mem_aloc_hdr)
{
	return 0;
}

static inline int   memblock_self_check_with_output(mmblock_hdr_t* mem_aloc_hdr,output_func output)
{
	return 0;
}

#endif /* END CONFIG_MEM_BLOCK_ALOCTOR */

#endif /* __ACO_MMBLOCK_H */
