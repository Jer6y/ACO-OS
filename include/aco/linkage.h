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
#define get_text_section_start()	get_section_start(text)
#define get_text_section_end()		get_section_end(text)
#define get_data_section_start()	get_section_start(data)
#define get_data_section_end()		get_section_end(data)
#define get_bss_section_start()		get_section_start(bss)
#define get_bss_section_end()		get_section_end(bss)
#define get_init_section_start()	get_section_start(init)
#define get_init_section_end()		get_section_end(init)

#define get_segment_start(seg) ({                       \
                                extern char __s##seg[]; \
                                __s##seg;    		\
                                })

#define get_segment_end(seg) ({                         \
                                extern char __e##seg[]; \
                                __e##seg;    		\
                             })

#define get_kernel_start()  ({                              	\
                              extern char _start_kernel[];	\
                              _start_kernel;   			\
                            })

#define get_kernel_end()    ({                              	\
                              extern char _end_kernel[];	\
                              _end_kernel;   			\
                            })
#else


#endif /* __ASSEMBLY__ */


#endif /* __ACO_LINKAGE_H */
