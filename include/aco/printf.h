#ifndef __ACO_PRINTK_H
#define __ACO_PRINTK_H

#include <generated/autoconf.h>

//porting implementation
#include <asm/printf.h>

#define printf  __ARCH_PRINTF

#define puts	__ARCH_PUTS

#define putchar	__ARCH_PUTCHAR

/* this is different from 'LOG_ASSERT'
 * why we need it is that log system only init
 * after mmu open , that means can't use log api
 * before mmu open, so we need a other api to
 * to the assertion
 */
#ifdef CONFIG_DEBUG_ASSERTION


#define ASSERT(condition)  	do	\
			  	{	\
					if(!(condition))							\
					{									\
						printf("[FILE %s] %d: "#condition"\n", __FILE__, __LINE__);	\
						__ARCH_ASSERT_ERROR(condition);					\
					}									\
				} while(0)

#else  /* NOT CONFIG_DEBUG_ASSERTION */

#define ASSERT(condition)	do {} while(0)

#endif /* END CONFIG_DEBUG_ASSERTION */


#endif /* END __ACO_PRINTK_H */
