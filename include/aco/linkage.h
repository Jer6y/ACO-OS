#ifndef __ACO_LINKAGE_H
#define __ACO_LINKAGE_H

#include <aco/link.h>

#ifndef __ASSEMBLY__

#define  USED			__attribute__((used))
#define  WEAK			__attribute__((weak))
#define  PUTINTO_SEGMENT(segnm)	__attribute__((section("."#segnm)))
#define  PUTINTO_INIT(prio)   	USED PUTINTO_SEGMENT(initcall##prio.init)

#define get_section_start(sec) ({				\
				char* __result;         	\
                                extern char __s##sec_section[]; \
                                __result = __s##sec_section;    \
                                __result;               	\
                                })

#define get_section_end(sec)	({                               \
                                char* __result;                 \
                                extern char __e##sec_section[]; \
                                __result = __e##sec_section;    \
                                __result;                       \
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
                                char* __result;         \
                                extern char __s##seg[]; \
                                __result = __s##seg;    \
                                __result;               \
                                })

#define get_segment_end(seg) ({                         \
                                char* __result;         \
                                extern char __e##seg[]; \
                                __result = __e##seg;    \
                                __result;               \
                                })

#define get_kernel_start()  ({                              \
                              char* __result;             \
                              extern char _start_kernel[];\
                              __result = _start_kernel;   \
                              __result;                   \
                            })

#define get_kernel_end()    ({                              \
                              char* __result;             \
                              extern char _end_kernel[];\
                              __result = _end_kernel;   \
                              __result;                   \
                            })
#else


#endif /* __ASSEMBLY__ */


#endif /* __ACO_LINKAGE_H */
