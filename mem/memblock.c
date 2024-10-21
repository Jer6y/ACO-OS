#include <mm/memblock_alloc.h>
#include <aco/errno.h>
#include <aco/types.h>

FUNC_BUILTIN void init_blk_hdr(block_hdr_t* blk_hdr)
{
	blk_hdr->magic_number = MAGIC_NUMBER;
	INIT_NODE(&(blk_hdr->_node));
}


int   memblock_allocator_init(mmblock_hdr_t* mem_aloc_hdr, uint64 pa, uint64 size, uint32 block_size)
{
	if(size % block_size  != 0)
		return -EINVAL;
	mem_aloc_hdr->pa_start = pa;
	mem_aloc_hdr->block_size = block_size;
	mem_aloc_hdr->total_block = (size / block_size);
	mem_aloc_hdr->rest_block = (size / block_size);
	INIT_LIST_HEAD(&(mem_aloc_hdr->_list_block));
	INIT_LIST_HEAD(&(mem_aloc_hdr->_list_mmaloct));
	for(int i=0;i<(mem_aloc_hdr->total_block);i++)
	{
		block_hdr_t* allocated_blk_hdr = (block_hdr_t*)( pa + block_size*i);
		init_blk_hdr(allocated_blk_hdr);
		list_add_tail(&(allocated_blk_hdr->_node),&(mem_aloc_hdr->_list_block));
	}
	return 0;
}

void* memblock_alloc(mmblock_hdr_t* mem_aloc_hdr)
{
	if(mem_aloc_hdr->rest_block <= 0)
		return NULL;
	block_hdr_t* blk_hdr = list_first_entry(&(mem_aloc_hdr->_list_block),block_hdr_t,_node);
	//ASSERT(blk_hdr->magic_number == MAGIC_NUMBER);
	list_del_init(&(blk_hdr->_node));
	mem_aloc_hdr->rest_block--;
	return (void*)blk_hdr;
}

void  memblock_free(mmblock_hdr_t* mem_aloc_hdr, void* address)
{
	if(mem_aloc_hdr->rest_block >= mem_aloc_hdr->total_block)
                return;
	init_blk_hdr((block_hdr_t*)address);
	list_add_tail(&(((block_hdr_t*)address)->_node),&(mem_aloc_hdr->_list_block));
	mem_aloc_hdr->rest_block++;
}

int   memblock_allocator_merge(mmblock_hdr_t* mem_allocator, mmblock_hdr_t* merged_mem_allocator)
{
	return 0;
}

