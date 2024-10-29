#ifndef __ACO_ASSERT_H
#define __ACO_ASSERT_H

#include <aco/printf.h>
#include <generated/autoconf.h>

// arch porting implementation
#include <asm/assert.h>

#ifdef CONFIG_DEBUG_ASSERTION

/*
 * Desc :  if condition is False,
 * 	   do error handler
 * Prarm:  
 * 	    contidition : the assertion condition
 *
 * Note :  Need Arch Implementation
 * 	   void __arch_assert_error(const char* condition);
 *
 */
#define ASSERT(condition)       do      \
                                {       \
                                        if(!(condition))                                                        \
                                        {                                                                       \
                                                printf("[FILE %s] %d: "#condition"\n", __FILE__, __LINE__);     \
                                                __arch_assert_error(#condition);                                 \
                                        }                                                                       \
                                } while(0)

#else  /* NOT CONFIG_DEBUG_ASSERTION */

#define ASSERT(condition)       do {} while(0)

#endif /* END CONFIG_DEBUG_ASSERTION */


#endif /*__ACO_ASSERT_H*/
