#ifndef __ACO_PAGE_H
#define __ACO_PAGE_H

/* port implementation */
#include <asm/page.h>

#define PAGE_OFFSET  		__ARCH_PAGE_OFFSET
#define PAGE_SIZE     		__ARCH_PAGE_SIZE
#define PAGE_ALIGN_BIT		((1ULL << PAGE_OFFSET) -1)
#define PAGE_UPPER_ALIGN(x)	(((x) + (PAGE_SIZE -1)) & (~PAGE_ALIGN))
#define PAGE_DOWNS_ALIGN(x)	((x) & (~PAGE_ALIGN))


#endif /* __ACO_PAGE_H */
