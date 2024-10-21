#ifndef __ACO_MMBLOCK_H
#define __ACO_MMBLOCK_H

#include <aco/types.h>
#include <aco/list.h>
//memory block allocator handler struct
//no lock for it
typedef struct mmblock_hdr_s {
	uintptr_t 	pa_start	;
	uint32	  	block_size	;
	uint32	  	total_block	;
	uint32 	  	rest_block	;
	struct list_head _list_block	; //_list_block hang the block_hdr node
	struct list_head _list_mmaloct	; //_list_mmaloct hang other mmblock allocator
} mmblock_hdr_t;

#define MAGIC_NUMBER 	0x2371
//one block handler, this is site in the
//the start address of the block want allocated
typedef struct block_hdr_s {
	uint16		  magic_number	;
	struct list_head  _node		;
} block_hdr_t;

int   memblock_allocator_init(mmblock_hdr_t* mem_aloc_hdr, uint64 pa, uint64 size, uint32 block_size);

void* memblock_alloc(mmblock_hdr_t* mem_aloc_hdr);

void  memblock_free(mmblock_hdr_t* mem_aloc_hdr, void* address);

int   memblock_allocator_merge(mmblock_hdr_t* mem_allocator, mmblock_hdr_t* merged_mem_allocator);

#endif /* __ACO_MMBLOCK_H */
