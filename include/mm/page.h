#ifndef __ACO_PAGE_H
#define __ACO_PAGE_H

#include <generated/autoconf.h>

#define PAGE_OFFSET  		((unsigned long)CONFIG_PAGE_OFFSET)
#define PAGE_SIZE     		((unsigned long)(1ULL << PAGE_OFFSET))
#define PAGE_ALIGN_BIT		((unsigned long)((1ULL << PAGE_OFFSET) -1))
#define PAGE_UPPER_ALIGN(x)	((unsigned long)(((unsigned long)(x) + (PAGE_SIZE -1)) & (~PAGE_ALIGN_BIT)))
#define PAGE_DOWNS_ALIGN(x)	((unsigned long)((unsigned long)(x) & (~PAGE_ALIGN_BIT)))

#define VAL_IS_PAGE_ALIGNED(val)	(((unsigned long)(val) & PAGE_ALIGN_BIT) == 0)

#define PA_PAGE_ALIGNED(pa)	VAL_IS_PAGE_ALIGNED(pa)
#define VA_PAGE_ALIGNED(va)	VAL_IS_PAGE_ALIGNED(va)


#endif /* __ACO_PAGE_H */
