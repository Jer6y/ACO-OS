#include "rttest.h"
#include <aco/errno.h>
#include <mm/vm.h>


#define CHECK(condition)        do                              \
                                {                               \
                                        if(condition)           \
                                                (*success)++;   \
                                        else                    \
                                                (*error)++;     \
                                } while(0)

int rt_vma_api(int* success, int* error)
{
	CHECK(PROT_V == 1);
	CHECK(PROT_R == 2);
	CHECK(PROT_W == 4);
	CHECK(PROT_X == 8);
	CHECK(PROT_U == 16);
	CHECK(PROT_G == 32);
	CHECK(PROT_A == 64);
	CHECK(PROT_D == 128);
	CHECK((vm_prot)(PROT_V | PROT_R | PROT_W | PROT_X | PROT_U | PROT_G | PROT_A | PROT_D) \
			== (uintptr_t)(PROT_V | PROT_R | PROT_W | PROT_X | PROT_U | PROT_G | PROT_A | PROT_D));
	pgt* pgtable = hw_get_current_pgtable();
	CHECK(pgtable != NULL);
	CHECK(VAL_IS_PAGE_ALIGNED(pgtable) == 1);
	CHECK((viraddr_t)pgtable >= PGFRAME_VA_START\
			&& (viraddr_t)pgtable < PGFRAME_VA_END);
	hw_load_pgtable(pgtable);
	CHECK(1==1);
	CHECK(hw_mmap(pgtable, 0x3000, 0x10000, PROT_V | PROT_R | PROT_W | PROT_X, 3*PAGE_SIZE) == 0);
	CHECK(hw_mmap(pgtable, 0x2000, 0x0f000, PROT_V | PROT_R | PROT_W | PROT_X, 5*PAGE_SIZE) == -EEXIST);
	hw_unmap(pgtable, 0x3000, PAGE_SIZE);
	CHECK(hw_mmap(pgtable, 0x3000, 0x10000, PROT_V | PROT_R | PROT_W | PROT_X, PAGE_SIZE) == 0);
	CHECK(hw_mmap(pgtable, 0x3000, 0x10000, PROT_V | PROT_R | PROT_W | PROT_X, PAGE_SIZE) == -EEXIST);
	hw_unmap(pgtable, 0x2000, 10*PAGE_SIZE);
#if (CONFIG_PAGE_OFFSET == 12)
	CHECK(hw_mmap(pgtable, 0x3000, 0x10000, PROT_V | PROT_R | PROT_W | PROT_X, 3*PAGE_SIZE) == 0);
	CHECK(hw_mmap(pgtable, 0x2000, 0x0f000, PROT_V | PROT_R | PROT_W | PROT_X, 5*PAGE_SIZE) == -EEXIST);
	CHECK(hw_mmap(pgtable, 0x2000, 0x0f000, PROT_V | PROT_R | PROT_W | PROT_X, 1*PAGE_SIZE) == 0);
        hw_unmap(pgtable, 0x2000, 4*PAGE_SIZE);
#endif
	vma_t* vma = vma_alloc();
	CHECK(vma != NULL);
	vma_free(NULL);
	CHECK(1 == 1);
	CHECK(vm_check(vma) == 0);
	CHECK(vm_iomap(vma, 0x1000, 0x1000, PROT_V | PROT_W | PROT_R, 20) == 0);
	CHECK(vm_check(vma) == 0);
	CHECK(vm_iounmap(vma, 0x1000, 0x2000) != 0);
	CHECK(vm_check(vma) == 0);
	CHECK(vm_iomap(vma, 0x1000, 0x3000, PROT_V | PROT_W | PROT_R, 20) == -EEXIST);
	CHECK(vm_check(vma) == 0);
	CHECK(vm_iomap(vma, 0x1000, 0x4000, PROT_V | PROT_W | PROT_R, PAGE_SIZE -1) == -EEXIST);
	CHECK(vm_check(vma) == 0);
	CHECK(vm_iomap(vma, 0x1000, 0x4000, PROT_V | PROT_W | PROT_R, PAGE_SIZE) == -EEXIST);
	CHECK(vm_check(vma) == 0);
	CHECK(vm_iounmap(vma, 0x1000, 30) == 0);
	CHECK(vm_check(vma) == 0);
	CHECK(vm_iomap(vma, 0x1000, 0x0, PROT_V | PROT_W | PROT_R, PAGE_SIZE*PGFRAME_SIZE) != 0);
	CHECK(vm_check(vma) == 0);
	CHECK(vm_iomap(NULL, 0x0, 0x0, PROT_V | PROT_W | PROT_R, PAGE_SIZE) == -EFAULT);
	CHECK(vm_iomap(vma, PGFRAME_VA_START + PAGE_SIZE , 0x0 , PROT_V | PROT_W | PROT_R, PAGE_SIZE) == -EPERM);
	CHECK(vm_iomap(vma, PGFRAME_VA_START , 0x0 , PROT_V | PROT_W | PROT_R, PAGE_SIZE) == -EPERM);
	CHECK(vm_iomap(vma, PGFRAME_VA_START - PAGE_SIZE, 0x0, PROT_V | PROT_W | PROT_R, 2*PAGE_SIZE) ==-EPERM);
	CHECK(vm_iomap(vma, PGFRAME_VA_END - PAGE_SIZE, 0x0, PROT_V | PROT_W | PROT_R, 2*PAGE_SIZE) ==-EPERM);
	CHECK(vm_iomap(vma, PGFRAME_VA_END - PAGE_SIZE, 0x0, PROT_V | PROT_W | PROT_R, PAGE_SIZE) ==-EPERM);
	CHECK(vm_iomap(vma, 0x0, PGFRAME_PA_START , PROT_V | PROT_W | PROT_R, PAGE_SIZE) == -EPERM);
	CHECK(vm_iomap(vma, 0x0, PGFRAME_PA_START-PAGE_SIZE , PROT_V | PROT_W | PROT_R, 2*PAGE_SIZE) == -EPERM);
	CHECK(vm_iomap(vma, 0x0, PGFRAME_PA_END -PAGE_SIZE , PROT_V | PROT_W | PROT_R, 2*PAGE_SIZE) == -EPERM);
	CHECK(vm_iomap(vma, 0x0, PGFRAME_PA_END -PAGE_SIZE , PROT_V | PROT_W | PROT_R, PAGE_SIZE) == -EPERM);
	for(int i=0;i<30;i++)
	{
		vma_t* tmp_vma = vma_alloc();
		CHECK(vm_iomap(tmp_vma, 0x0, 0x1000, PROT_V | PROT_R, PAGE_SIZE) == 0);
		CHECK(vma_cache_number() ==  INIT_VMA_NUM -2);
		CHECK(vm_iomap(tmp_vma, 0x1000, 0x2000, PROT_V | PROT_R, PAGE_SIZE) == 0);
		CHECK(vm_iomap(tmp_vma, 0x2000, 0x3000, PROT_V | PROT_R, PAGE_SIZE) == 0);
		CHECK(vm_iomap(tmp_vma, 0x3000, 0x4000, PROT_V | PROT_R, PAGE_SIZE) == 0);
		CHECK(vm_iomap(tmp_vma, 0x4000, 0x5000, PROT_V | PROT_R, PAGE_SIZE) == 0);
		CHECK(vm_iomap(tmp_vma, 0x5000, 0x6000, PROT_V | PROT_R, PAGE_SIZE) == 0);
		CHECK(vm_iomap(tmp_vma, 0x6000, 0x7000, PROT_V | PROT_R, PAGE_SIZE) == 0);
		CHECK(vm_iounmap(tmp_vma, 0x2000, 2*PAGE_SIZE) == -ENOBUFS);
		CHECK(vm_iounmap(tmp_vma, 0x1000, 2*PAGE_SIZE) == -ENOBUFS);
		CHECK(vm_iounmap(tmp_vma, 0x1000, PAGE_SIZE) == 0);
		CHECK(vm_iounmap(tmp_vma, 0x2000, PAGE_SIZE) == 0);
		CHECK(vm_iounmap(tmp_vma, 0x1000, PAGE_SIZE) == -ENOBUFS);
		CHECK(vm_iounmap(tmp_vma, 0x1010, PAGE_SIZE) == -EFAULT);
		CHECK(vm_iomap(tmp_vma, 0x3, 0x1000, PROT_V | PROT_R, PAGE_SIZE) == -EFAULT);
		CHECK(vm_iomap(tmp_vma, 0x0, 0x1020, PROT_V | PROT_R, PAGE_SIZE) == -EFAULT);
		vma_free(tmp_vma);
	}
	vma_free(vma);
	CHECK(1 == 1);
	CHECK(vma_cache_number() == INIT_VMA_NUM);
	CHECK(vma_area_cache_number() == INIT_VMA_AREA_NUM);
	return 0;
}
