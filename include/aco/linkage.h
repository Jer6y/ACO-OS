#ifndef __ACO_LINKAGE_H
#define __ACO_LINKAGE_H

#include <aco/link.h>

#ifndef __ASSEMBLY__

#define  PACKED			__attribute__((packed))
#define  ALIGN(x)		__attribute__((aligned(x)))
#define  USED			__attribute__((used))
#define  WEAK			__attribute__((weak))
#define  PUTINTO_SEGMENT(segnm)	__attribute__((section("."#segnm)))
#define  PUTINTO_INIT(prio)   	USED PUTINTO_SEGMENT(initcall##prio.init)

#define get_section_start(ection) 	({				\
                                	extern char __s##ection##_section[]; \
                                 	__s##ection##_section;    \
                                	})

#define get_section_end(ection)		({                               \
                                	extern char __e##ection##_section[]; \
                                	__e##ection##_section;    \
                                	})

#define get_section_size(ection)	({									\
						char* start_##ection = get_section_start(ection);		\
						char* end_##ection   = get_section_end(ection);			\
						size_t __result_size = 	(size_t)(end_##ection - start_##ection);\
						__result_size;							\
					})

#define get_segment_start(seg) ({                       \
                                extern char __s##seg[]; \
                                __s##seg;    		\
                                })

#define get_segment_end(seg) ({                         \
                                extern char __e##seg[]; \
                                __e##seg;    		\
                             })

#define get_segment_size(seg)	({			\
					char* start_##seg = get_segment_start(seg);\
					char* end_##seg   = get_segment_end(seg);\
					size_t __result_size =  (size_t)(end_##seg - start_##seg);\
					__result_size;                                           \
				})


#define get_kernel_start()  ({                              	\
                              extern char _start_kernel[];	\
                              _start_kernel;   			\
                            })

#define get_symbol_offset(sym)	({\
				char* kernel_start = get_kernel_start()\
				extern char sym[];\
				size_t __offset = (size_t)(sym - kernel_start);\
				__offset;\
				})

#define get_kernel_end()    ({                              	\
                              extern char _end_kernel[];	\
                              _end_kernel;   			\
                            })
#else


#endif /* __ASSEMBLY__ */


#endif /* __ACO_LINKAGE_H */
