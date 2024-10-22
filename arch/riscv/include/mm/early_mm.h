#ifndef __RISCV_EARLY_MM_H
#define __RISCV_EARLY_MM_H

#include <mm/memblock_alloc.h>

void* mem_alloc();

void  mem_free(void* address);

int   setup_earlymm();

// this function is very special.
// we use memblock allocator to manage
// our initheap memory before mmu open in riscv.
// such memblock allocator is helpful when we want
// to setup our mmu, but when mmu open,
// all the memory managed by early memblock allocator
// can not be used anymore, it will leads a 
// very low memory usage , so our refresh_for_mmu() functions come out
// call it before we load mmu page table to 'satp' register
// and if you use it , never use mem_alloc or mem_free anymore 
// until you open mmu
int   refresh_for_mmu();

int  mem_test(void);

#endif /* __RISCV_EARLY_MM_H */
