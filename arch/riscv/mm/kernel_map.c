#include <mm/kernel_map.h>
#include <aco/types.h>
#include <aco/linkage.h>
#include <aco/libfdt.h>
#include <aco/printf.h>
#include <aco/assert.h>
#include <aco/string.h>

struct kernel_mapping __map_of_kernel;

#define CHECK(condition) do {\
				if(condition)\
					(*success)++;\
				else\
					(*error)++;\
			    } while(0);

static int riscv_kernel_map_test(int* success, int* error)
{
	uintptr_t ks_addrs = __map_of_kernel.pa_st_kernel;
	uintptr_t ms_addrs = __map_of_kernel.pa_st_memory;
	size_t	  k_size   = __map_of_kernel.size_kernel;
	size_t    m_size   = __map_of_kernel.size_memory;
	uintptr_t ke_addrs = ks_addrs + (uintptr_t)k_size;
	uintptr_t me_addrs = ms_addrs + (uintptr_t)m_size; 
	CHECK(k_size < m_size);
	CHECK(ks_addrs >= ms_addrs && ks_addrs <  me_addrs);
	CHECK(ke_addrs >= ms_addrs && ke_addrs <  me_addrs);
	CHECK(ks_addrs < KERNEL_OFFSET);
	CHECK(ke_addrs < KERNEL_OFFSET);
	CHECK(ms_addrs < KERNEL_OFFSET);
	CHECK(me_addrs < KERNEL_OFFSET);
#ifdef CONFIG_QEMU_RV64
	CHECK(ks_addrs == 0x80200000);
	CHECK(ms_addrs == 0x80000000);
#endif 
	return 0;
}


void kernel_map_init(uintptr_t dtb_address)
{
	uintptr_t tmp;
	int len,offset,address_cells,size_cells;
	const void* prop;
	uintptr_t address,size;
	__map_of_kernel.pa_st_kernel = (phyaddr_t)(get_kernel_start());
	tmp = (uintptr_t)(get_kernel_end());
	__map_of_kernel.size_kernel =  (tmp - __map_of_kernel.pa_st_kernel);
	__map_of_kernel.va2pa_offset = (KERNEL_OFFSET - __map_of_kernel.pa_st_kernel);
	tmp = fdt_check_header((void*)dtb_address);
        ASSERT(tmp ==0);
	offset = fdt_node_offset_by_prop_value((void*)dtb_address, -1, "device_type", "memory", sizeof("memory"));
	ASSERT(offset >=0 );
	address_cells = fdt_address_cells((void*)dtb_address, offset);
	ASSERT(address_cells >=0);
	size_cells = fdt_size_cells((void*)dtb_address, offset);
	ASSERT(size_cells >=0);
	address_cells = (address_cells > size_cells)?address_cells:size_cells;
	size_cells = address_cells;
	prop = fdt_getprop((void*)dtb_address, offset, "reg", &len);
	ASSERT(prop != NULL);
	memcpy(&address, prop, sizeof(uint32_t)*address_cells);
	memcpy(&size, prop + sizeof(uint32_t)*address_cells, sizeof(uint32_t)* size_cells);
	if(address_cells == 2)
		address = fdt64_to_cpu(address);
	else
		address = fdt32_to_cpu(address);

	if(size_cells ==2)
		size 	= fdt64_to_cpu(size);
	else
		size	= fdt32_to_cpu(size);

	__map_of_kernel.pa_st_memory = (phyaddr_t)address;
	__map_of_kernel.size_memory  = size;
	__map_of_kernel.pa_st_free   = RISCV_KERNEL_PA_END;
	__map_of_kernel.size_free    = (RISCV_MEMORY_PA_END - RISCV_KERNEL_PA_END);
	
	printf("[RV_MM] : va2paoffset   numbers %x\n",RISCV_VA2PA_OFFSET);
	printf("[RV_MM] : physical memory start %p\n",RISCV_MEMORY_PA_START);
	printf("[RV_MM] : physical memory end   %p\n",RISCV_MEMORY_PA_END);
	printf("[RV_MM] : virtual  memory start %p\n",RISCV_MEMORY_VA_START);
	printf("[RV_MM] : virtual  memory end   %p\n",RISCV_MEMORY_VA_END);
	printf("[RV_MM] : phy/vir  memory size  %x\n",RISCV_MEMORY_SIZE);
	printf("[RV_MM] : kernel physical start %p\n",RISCV_KERNEL_PA_START);
	printf("[RV_MM] : kernel physical end   %p\n",RISCV_KERNEL_PA_END);
	printf("[RV_MM] : kernel virtual  start %p\n",RISCV_KERNEL_VA_START);
	printf("[RV_MM] : kernel virtual  end   %p\n",RISCV_KERNEL_VA_END);
	printf("[RV_MM] : kernel memory   size  %x\n",RISCV_KERNEL_SIZE);
	printf("[RV_MM] : physical fremem start %p\n",RISCV_FREE_PA_START);
	printf("[RV_MM] : physical fremem end   %p\n",RISCV_FREE_PA_END);
	printf("[RV_MM] : virtual  fremem start %p\n",RISCV_FREE_VA_START);
	printf("[RV_MM] : virtual  fremem end   %p\n",RISCV_FREE_VA_END);
	printf("[RV_MM] : vir/phy  fremem size  %x\n",RISCV_FREE_SIZE);
	int suc =0;
	int err =0;
	riscv_kernel_map_test(&suc,&err);
	printf("[RV_MM] : kernel_map test pass  %d/%d\n",suc,suc+err);
	return;
}

#include <mm/page.h>
void* freemem_alloc_pa(void)
{
	if(RISCV_FREE_SIZE < PAGE_SIZE)
		return NULL;
	void* ret = (void*)(__map_of_kernel.pa_st_free);
	__map_of_kernel.pa_st_free   += PAGE_SIZE;
        __map_of_kernel.size_free    -= PAGE_SIZE;
	/*
	printf("[RV_MM] : some one ask for alloc \n");
	printf("[RV_MM] : physical fremem start %p\n",RISCV_FREE_PA_START);
        printf("[RV_MM] : physical fremem end   %p\n",RISCV_FREE_PA_END);
        printf("[RV_MM] : virtual  fremem start %p\n",RISCV_FREE_VA_START);
        printf("[RV_MM] : virtual  fremem end   %p\n",RISCV_FREE_VA_END);
        printf("[RV_MM] : vir/phy  fremem size  %x\n",RISCV_FREE_SIZE);
	*/
	return ret;
}
