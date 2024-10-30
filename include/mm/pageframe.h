#ifndef __ACO_PAGEFRAME_H
#define __ACO_PAGEFRAME_H

#include <aco/types.h>
#include <aco/spinlock.h>
#include <mm/page.h>
#include <aco/linkage.h>
#include <aco/list.h>

//porting implementation
#include <asm/pageframe.h>

//pageframe 页帧管理思想
//1. 管理一段连续的页
//2. 只支持虚拟地址到物理地址的线性映射
//3. 默认认为 虚拟地址 大于 物理地址
//4. 初始化只允许使能一块连续的页作为空闲页，其他默认被初始化占用

//管理的连续页的物理起始地址, 必须是页对齐的
//需要对应给出 arch 里面的实现
#define   PGFRAME_PA_START	__ARCH_PGFRAME_PA_START

//管理的连续页的虚拟起始地址，必须是页对齐的
//需要对应给出 arch 里面的实现
#define   PGFRAME_VA_START	__ARCH_PGFRAME_VA_START

//管理的连续页的大小，必须是页对齐，同时 大于0
//需要对应给出 arch 里面的实现
#define   PGFRAME_SIZE		__ARCH_PGFRAME_SIZE

//管理的一个页帧的大小(单位 byte)
#define   PGFRAME_PGSIZE        PAGE_SIZE

#define   PGFRAME_PA_END	(phyaddr_t)((phyaddr_t)PGFRAME_PA_START + (uintptr_t)PGFRAME_SIZE)
#define   PGFRAME_VA_END	(viraddr_t)((viraddr_t)PGFRAME_VA_START + (uintptr_t)PGFRAME_SIZE)
#define   PGFRAME_VA2PA_OFFSET	(uintptr_t)((uintptr_t)PGFRAME_VA_START - (uintptr_t)PGFRAME_PA_START)
#define   PGFRAME_VA2PA(adpgfm)	(phyaddr_t)((uintptr_t)(adpgfm) - PGFRAME_VA2PA_OFFSET)
#define   PGFRAME_PA2VA(adpgfm) (viraddr_t)((uintptr_t)(adpgfm) + PGFRAME_VA2PA_OFFSET)
#define   PGFRAME_PA2START_OFFSET(adpgfm)	(uintptr_t)((phyaddr_t)(adpgfm) - (phyaddr_t)(PGFRAME_PA_START))
#define   PGFRAME_PA2PAGEID(adpgfm)		(uintptr_t)(PGFRAME_PA2START_OFFSET(adpgfm) / (uintptr_t)PGFRAME_PGSIZE)
#define   PGFRAME_VA2START_OFFSET(adpgfm)	(uintptr_t)((viraddr_t)(adpgfm) - (viraddr_t)(PGFRAME_VA_START))
#define   PGFRAME_VA2PAGEID(adpgfm)		(uintptr_t)(PGFRAME_VA2START_OFFSET(adpgfm) / (uintptr_t)PGFRAME_PGSIZE)
#define   PGFRAME_PAGEID2VA(id)			(viraddr_t)((viraddr_t)(PGFRAME_VA_START) + (uintptr_t)((id)*(uintptr_t)PGFRAME_PGSIZE))
#define   PGFRAME_PAGEID2PA(id)			(phyaddr_t)((phyaddr_t)(PGFRAME_PA_START) + (uintptr_t)((id)*(uintptr_t)PGFRAME_PGSIZE))
#define   PGFRAME_PAGE_NUMS			((size_t)((size_t)PGFRAME_SIZE / (size_t)PGFRAME_PGSIZE))


//管理的页帧里面内核所在的物理地址起始位置
//需要对应给出 arch 里面的实现
#define   PGFRAME_PA_KSTART	__ARCH_PGFRAME_PA_KSTART

//管理的页帧里面内核所在的虚拟地址起始位置
#define   PGFRAME_VA_KSTART	PGFRAME_PA2VA(PGFRAME_PA_KSTART)

//管理的页帧里面内核的整体大小
#define   PGFRAME_K_SIZE	({	\
					char* kernel_start = get_kernel_start();\
					char* kernel_end = get_kernel_end();\
					(uintptr_t)(kernel_end - kernel_start);\
				})

//管理的页帧里面使能的空闲地址开始
#define   PGFRAME_FREE_VA_START	__ARCH_PGFRAME_FREE_VA_START

//管理的页帧里面使能空闲地址长度
#define   PGFRAME_FREE_SIZE	__ARCH_PGFRAME_FREE_SIZE

typedef enum   pgfm_type {
	PAGE_BUDDYALLOCATOR  = 0x2971u,
} pgfm_type_t;

typedef struct  pageframe {
	spinlock_t 		lk;
	pgfm_type_t		pgtype;
	union  {
		struct PACKED  {   //buddy allocator meta data
			#define  PG_BUDDY_FLAG_FREE  	(1ULL <<0)
			#define	 PG_BUDDY_FLAG_HEAD  	(1ULL <<1)
		        #define  PG_BUDDY_FLAG_BODY  	(1ULL <<2)
			#define  PG_BUDDY_FLAG_STATIC 	(1ULL <<3)
			uint8_t		  buddy_flags;
			struct list_head  buddy_node;
			uint8_t           buddy_order;
		};
	} meta;
} pageframe_t;

extern struct pageframe* pages;
#define PAGES pages

#define va2pa(va)	((phyaddr_t)(PGFRAME_VA2PA(va)))
#define pa2va(pa)	((viraddr_t)(PGFRAME_PA2VA(pa)))
#define va2id(va)	((int)(PGFRAME_VA2PAGEID(va)))
#define id2va(id)	((viraddr_t)(PGFRAME_PAGEID2VA(id)))
#define pa2id(pa)	((int)(PGFRAME_PA2PAGEID(pa)))
#define id2pa(id)	((phyaddr_t)(PGFRAME_PAGEID2PA(id)))
#define va2pg(va)	(pages + va2id(va))
#define pg2va(pg)	(id2va((int)((struct pageframe*)(pg) - pages)))
#define pa2pg(pa)	(pages + pa2id(pa))
#define pg2pa(pg)	(id2pa((int)((struct pageframe*)(pg) - pages)))
#define id2pg(id)	(pages + id)
#define pg2id(pg)	((int)((struct pageframe*)(pg) - pages))
#define pa_valid(pa)	((phyaddr_t)(pa) >= (phyaddr_t)PGFRAME_PA_START && (phyaddr_t)(pa) < PGFRAME_PA_END)
#define va_valid(va)	((viraddr_t)(va) >= (viraddr_t)PGFRAME_VA_START && (viraddr_t)(va) < PGFRAME_VA_END)
#define id_valid(id)	((int)(id) >=0 && (int)(id) <  (int)((size_t)PGFRAME_SIZE/(size_t)PGFRAME_PGSIZE))
#define pg_valid(pg)	(id_valid(pg2id(pg)))


int pageframe_init(void);

int pages_slfcheck(void);

#endif /* __ACO_PAGEFRAME_H */
