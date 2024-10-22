#include <mm/memblock_alloc.h>
#include <aco/errno.h>
#include <aco/types.h>

FUNC_BUILTIN void init_blk_hdr(block_hdr_t* blk_hdr)
{
	blk_hdr->magic_number = MAGIC_NUMBER;
	INIT_NODE(&(blk_hdr->_node));
}


FUNC_BUILTIN int __memblock_self_check(mmblock_hdr_t* mem_aloc_hdr, bool is_output, output_func output)
{
	if(mem_aloc_hdr == NULL)
	{	
		if(is_output)
			output("[MM] : Null Input Mem Allocator!\n");
		return -EINVAL;
	}
	if(is_output)
	{
		output("=======================Allocator Information====================\n");
		output("[MM] : Allocator Address %p\n",mem_aloc_hdr);
		output("[MM] : Allocator TotaBlk %d\n",mem_aloc_hdr->total_block);
		output("[MM] : Allocator RestBlk %d\n",mem_aloc_hdr->rest_block);
		output("[MM] : Allocator BlkSize %d\n",mem_aloc_hdr->block_size);
		output("[MM] : Allocator PaStart %p\n",mem_aloc_hdr->pa_start);
		output("================================================================\n");
	}
	if(mem_aloc_hdr->total_block < mem_aloc_hdr->rest_block)
	{
		if(is_output)
			output("[MM] : Total block < Rest block!\n");
		return -EFAULT;
	}
	struct list_head* iterator;
	int i =0;
	list_for_each(iterator, &(mem_aloc_hdr->_list_block))
	{
		block_hdr_t* blk = list_entry(iterator, block_hdr_t, _node);
		if(blk->magic_number != MAGIC_NUMBER)
		{
			if(is_output)
				output("[MM] : Threr is a error magic number block in list!\n");
			return -EFAULT;
		}
		/*
		if(is_output)
			output("[MM] : Block Id %d , Address %p\n", i, (uintptr_t)blk);
		*/
		i++;
	}
	if(i != mem_aloc_hdr->rest_block)
	{
		if(is_output)
			output("[MM] : error calculate for rest_block\n");
		return -EFAULT;
	}
	return 0;
}

int memblock_self_check(mmblock_hdr_t* mem_aloc_hdr)
{
	return __memblock_self_check(mem_aloc_hdr, 0, NULL);
}

int   memblock_self_check_with_output(mmblock_hdr_t* mem_aloc_hdr,output_func output)
{
	return __memblock_self_check(mem_aloc_hdr, 1, output);
}

FUNC_BUILTIN int foreach_nodes(mmblock_hdr_t* mem_aloc_hdr, foreach_func iterate_func, bool is_safe, bool continue_when_error)
{
	int ret = 0;
	struct list_head* ptr;
	if(is_safe)
	{
		struct list_head* safe;
		list_for_each_safe(ptr, safe, &(mem_aloc_hdr->_list_block))
		{
			block_hdr_t* blk = list_entry(ptr, block_hdr_t, _node);
			ret = iterate_func(blk);
			if(ret != 0)
			{
				if(!continue_when_error)
					return ret;
				else
					ret++;
			}
		}
	}
	else
	{
		list_for_each(ptr, &(mem_aloc_hdr->_list_block))
		{
			block_hdr_t* blk = list_entry(ptr, block_hdr_t, _node);
                        ret = iterate_func(blk);
                        if(ret != 0)
                        {
                                if(!continue_when_error)
                                        return ret;
                                else
                                        ret++;
                        }
		}
	}
	return ret;
}

int   memblock_for_each(mmblock_hdr_t* mem_aloc_hdr, foreach_func iterate_func)
{
	int ret;
	if(mem_aloc_hdr == NULL || iterate_func == NULL)
		return -EINVAL;
	ret = foreach_nodes(mem_aloc_hdr, iterate_func, false, false);
	if(ret)
		return ret;
	return 0;
}

int   memblock_for_each_safe(mmblock_hdr_t* mem_aloc_hdr, foreach_func iterate_func)
{
	int ret;
	if(mem_aloc_hdr == NULL || iterate_func == NULL)
                return -EINVAL;
	ret = foreach_nodes(mem_aloc_hdr, iterate_func, true, false);
        if(ret)
                return ret;
        return 0;
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

