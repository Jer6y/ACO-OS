#include "rttest.h"
#include <aco/errno.h>
#include <mm/vm.h>
#include <mm/buddy.h>

#define CHECK(condition)        do                              \
                                {                               \
                                        if(condition)           \
                                                (*success)++;   \
                                        else                    \
                                                (*error)++;     \
                                } while(0)
DEFINE_ATOMIC(smp_vma_test_done,0);
int rt_vma_api(int* success, int* error)
{
	if(atomic_fetch_and_add(&smp_vma_test_done,0) == 1)
	{
		vm_iomap(KVMA, 0x1000, 0x20000, PROT_V | PROT_R, PAGE_SIZE);
		CHECK(vm_check(KVMA) == 0);
		vm_iounmap(KVMA, 0x1000, PAGE_SIZE);
		CHECK(vm_check(KVMA) == 0);
		vm_iomap(KVMA, 0x1000+PAGE_SIZE, 0x20000+PAGE_SIZE, PROT_V | PROT_R, PAGE_SIZE);
		CHECK(vm_check(KVMA) == 0);
		vm_iounmap(KVMA, 0x1000+PAGE_SIZE, PAGE_SIZE);
		CHECK(vm_check(KVMA) == 0);
		vm_iomap(KVMA, 0x1000+2*PAGE_SIZE, 0x20000+2*PAGE_SIZE, PROT_V | PROT_R, PAGE_SIZE);
		CHECK(vm_check(KVMA) == 0);
		vm_iounmap(KVMA, 0x1000+2*PAGE_SIZE, PAGE_SIZE);
		CHECK(vm_check(KVMA) == 0);
		vm_iomap(KVMA, 0x1000+3*PAGE_SIZE, 0x20000+3*PAGE_SIZE, PROT_V | PROT_R, PAGE_SIZE);
		CHECK(vm_check(KVMA) == 0);
		vm_iounmap(KVMA, 0x1000+3*PAGE_SIZE, PAGE_SIZE);
		CHECK(vm_check(KVMA) == 0);
		vm_iomap(KVMA, 0x1000+4*PAGE_SIZE, 0x20000+4*PAGE_SIZE, PROT_V | PROT_R, PAGE_SIZE);
		CHECK(vm_check(KVMA) == 0);
		vm_iounmap(KVMA, 0x1000+4*PAGE_SIZE, PAGE_SIZE);
		CHECK(vm_check(KVMA) == 0);
		vm_mmap(KVMA, 0x1000, 0, PROT_V | PROT_R);
		CHECK(vm_check(KVMA) == 0);
		vm_unmap(KVMA, 0x1000, 0);
		CHECK(vm_check(KVMA) == 0);
		vm_mmap(KVMA, 0x1000+PAGE_SIZE, 0, PROT_V | PROT_R);
		CHECK(vm_check(KVMA) == 0);
		vm_unmap(KVMA, 0x1000+PAGE_SIZE, 0);
		CHECK(vm_check(KVMA) == 0);
		vm_mmap(KVMA, 0x1000+2*PAGE_SIZE, 0, PROT_V | PROT_R);
		CHECK(vm_check(KVMA) == 0);
		vm_unmap(KVMA, 0x1000+2*PAGE_SIZE, 0);
		CHECK(vm_check(KVMA) == 0);
		vm_mmap(KVMA, 0x1000+3*PAGE_SIZE, 0, PROT_V | PROT_R);
		CHECK(vm_check(KVMA) == 0);
		vm_unmap(KVMA, 0x1000+3*PAGE_SIZE, 0);
		CHECK(vm_check(KVMA) == 0);
		vm_mmap(KVMA, 0x1000+4*PAGE_SIZE, 0, PROT_V | PROT_R);
		CHECK(vm_check(KVMA) == 0);
		vm_unmap(KVMA, 0x1000+4*PAGE_SIZE, 0);
		CHECK(vm_check(KVMA) == 0);
		CHECK(vma_cache_number() == INIT_VMA_NUM);
	        CHECK(vma_area_cache_number() == INIT_VMA_AREA_NUM);	
		return 0;		
	}
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
	CHECK(vm_check(vma) == 0);
	CHECK(vm_iomap(vma, PGFRAME_VA_START + PAGE_SIZE , 0x0 , PROT_V | PROT_W | PROT_R, PAGE_SIZE) == -EPERM);
	CHECK(vm_check(vma) == 0);
	CHECK(vm_iomap(vma, PGFRAME_VA_START , 0x0 , PROT_V | PROT_W | PROT_R, PAGE_SIZE) == -EPERM);
	CHECK(vm_check(vma) == 0);
	CHECK(vm_iomap(vma, PGFRAME_VA_START - PAGE_SIZE, 0x0, PROT_V | PROT_W | PROT_R, 2*PAGE_SIZE) ==-EPERM);
	CHECK(vm_check(vma) == 0);
	CHECK(vm_iomap(vma, PGFRAME_VA_END - PAGE_SIZE, 0x0, PROT_V | PROT_W | PROT_R, 2*PAGE_SIZE) ==-EPERM);
	CHECK(vm_check(vma) == 0);
	CHECK(vm_iomap(vma, PGFRAME_VA_END - PAGE_SIZE, 0x0, PROT_V | PROT_W | PROT_R, PAGE_SIZE) ==-EPERM);
	CHECK(vm_check(vma) == 0);
	CHECK(vm_iomap(vma, 0x0, PGFRAME_PA_START , PROT_V | PROT_W | PROT_R, PAGE_SIZE) == -EPERM);
	CHECK(vm_check(vma) == 0);
	CHECK(vm_iomap(vma, 0x0, PGFRAME_PA_START-PAGE_SIZE , PROT_V | PROT_W | PROT_R, 2*PAGE_SIZE) == -EPERM);
	CHECK(vm_check(vma) == 0);
	CHECK(vm_iomap(vma, 0x0, PGFRAME_PA_END -PAGE_SIZE , PROT_V | PROT_W | PROT_R, 2*PAGE_SIZE) == -EPERM);
	CHECK(vm_check(vma) == 0);
	CHECK(vm_iomap(vma, 0x0, PGFRAME_PA_END -PAGE_SIZE , PROT_V | PROT_W | PROT_R, PAGE_SIZE) == -EPERM);
	CHECK(vm_check(vma) == 0);
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
	CHECK(vm_mmap(NULL,0x0,0,PROT_V | PROT_W) == -EFAULT);
	CHECK(vm_mmap(KVMA,0x1,0,PROT_V | PROT_W) == -EFAULT);
	CHECK(vm_check(KVMA) == 0);
	CHECK(vm_mmap(KVMA,4095,0,PROT_V | PROT_W) == -EFAULT);
	CHECK(vm_check(KVMA) == 0);
	CHECK(vm_mmap(KVMA,0x2000, MAX_ORDER,PROT_V | PROT_W) == -EFAULT);
	CHECK(vm_check(KVMA) == 0);
	CHECK(vm_mmap(KVMA,0x2000, MAX_ORDER+1,PROT_V | PROT_W) == -EFAULT);
	CHECK(vm_check(KVMA) == 0);
	CHECK(vm_mmap(KVMA,0x2000, MAX_ORDER+10,PROT_V | PROT_W) == -EFAULT);
	CHECK(vm_check(KVMA) == 0);
	CHECK(vm_mmap(KVMA,0x1000, 0, PROT_V | PROT_W | PROT_R) == 0);
	CHECK(vm_check(KVMA) == 0);
	char* ptr = (char*)0x1000;
	for(int i=0;i<PAGE_SIZE;i++)
	{
		ptr[i] = 0x57;
		CHECK(ptr[i] == 0x57);
	}
	CHECK(vm_unmap(NULL, 0x1000, 0) == -EFAULT);
	CHECK(vm_unmap(KVMA, 0x1000, MAX_ORDER) == -EFAULT);
	CHECK(vm_check(KVMA) == 0);
	CHECK(vm_unmap(KVMA, 0x1000, MAX_ORDER+1) == -EFAULT);
	CHECK(vm_check(KVMA) == 0);
	int ret = vm_unmap(KVMA, 0x1000, 1);
	CHECK(ret == -ENOBUFS || ret == -EFAULT);
	CHECK(vm_check(KVMA) == 0);
	CHECK(vm_mmap(KVMA, 0x1000, 0 , PROT_V | PROT_W | PROT_R) == -EEXIST);
	CHECK(vm_check(KVMA) == 0);
	CHECK(vm_mmap(KVMA, 0x10000, 0, PROT_V | PROT_R) == 0);
	CHECK(vm_check(KVMA) == 0);
	CHECK(vm_mmap(KVMA,PGFRAME_VA_START, 0, PROT_V) == -EEXIST);
	CHECK(vm_check(KVMA) == 0);
	vma = vma_alloc();
	CHECK(vma != NULL);
	CHECK(vm_check(vma) == 0);
	CHECK(vm_mmap(vma, 0x1000, 0, PROT_V) == -EEXIST);
	CHECK(vm_check(vma) == 0);
	CHECK(vm_unmap(vma, 0x1000, 0) == 0);
	CHECK(vm_check(vma) == 0);
	vma_t* vma_2 = vma_dup(vma);
	CHECK(vma_2 != NULL);
	CHECK(vm_check(vma_2) == 0);
	CHECK(vm_unmap(vma, 0x10000, 0) == 0);
	CHECK(vm_check(vma) == 0);
	vma_free(vma);
	vma_free(vma_2);
	CHECK(1 == 1);
	CHECK(vm_unmap(KVMA,0x1000, 0) == 0);
#if (CONFIG_BUDDYSYSTEM_MAX_ORDER >=3)
	CHECK(vm_mmap(KVMA,0x1000, 2, PROT_V) == 0);
	CHECK(vm_mmap(KVMA,0x1000, 0, PROT_V) == -EEXIST);
	CHECK(vm_unmap(KVMA, 0x1000, 2) == 0);
#endif
	CHECK(vm_unmap(KVMA, 0x10000, 0) == 0);
	CHECK(vma_cache_number() == INIT_VMA_NUM);
        CHECK(vma_area_cache_number() == INIT_VMA_AREA_NUM);
	atomic_fetch_and_add(&smp_vma_test_done,1);
	return 0;
}
