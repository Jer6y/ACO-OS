#ifndef __ACO_BYTE_ORDER_H
#define __ACO_BYTE_ORDER_H

#include <aco/types.h>

//porting implementation
#include <asm/byteorder.h>
// define __ARCH_BIG_ENDIAN or __ARCH_LITTLE_ENDIAN

/*
 * casts are necessary for constants, because we never know how for sure
 * how U/UL/ULL map to u16, u32, u64. At least not in a portable way.
 */
#define ___constant_swab16(x) ((uint16_t)(                         \
        (((uint16_t)(x) & (uint16_t)0x00ffU) << 8) |                  \
        (((uint16_t)(x) & (uint16_t)0xff00U) >> 8)))

#define ___constant_swab32(x) ((uint32_t)(                         \
        (((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24) |            \
        (((uint32_t)(x) & (uint32_t)0x0000ff00UL) <<  8) |            \
        (((uint32_t)(x) & (uint32_t)0x00ff0000UL) >>  8) |            \
        (((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24)))

#define ___constant_swab64(x) ((uint64_t)(                         \
        (((uint64_t)(x) & (uint64_t)0x00000000000000ffULL) << 56) |   \
        (((uint64_t)(x) & (uint64_t)0x000000000000ff00ULL) << 40) |   \
        (((uint64_t)(x) & (uint64_t)0x0000000000ff0000ULL) << 24) |   \
        (((uint64_t)(x) & (uint64_t)0x00000000ff000000ULL) <<  8) |   \
        (((uint64_t)(x) & (uint64_t)0x000000ff00000000ULL) >>  8) |   \
        (((uint64_t)(x) & (uint64_t)0x0000ff0000000000ULL) >> 24) |   \
        (((uint64_t)(x) & (uint64_t)0x00ff000000000000ULL) >> 40) |   \
        (((uint64_t)(x) & (uint64_t)0xff00000000000000ULL) >> 56)))

#define ___constant_swahw32(x) ((uint32_t)(                        \
        (((uint32_t)(x) & (uint32_t)0x0000ffffUL) << 16) |            \
        (((uint32_t)(x) & (uint32_t)0xffff0000UL) >> 16)))

#define ___constant_swahb32(x) ((uint32_t)(                        \
        (((uint32_t)(x) & (uint32_t)0x00ff00ffUL) << 8) |             \
        (((uint32_t)(x) & (uint32_t)0xff00ff00UL) >> 8)))



#if defined(__ARCH_BIG_ENDIAN)

#define __cpu_to_le64(x) ___constant_swab64((uint64_t)(x))
#define __le64_to_cpu(x) ___constant_swab64((uint64_t)(x))
#define __cpu_to_le32(x) ___constant_swab32((uint32_t)(x))
#define __le32_to_cpu(x) ___constant_swab32((uint32_t)(x))
#define __cpu_to_le16(x) ___constant_swab16((uint16_t)(x))
#define __le16_to_cpu(x) ___constant_swab16((uint16_t)(x))

#define __cpu_to_be64(x) ((uint64_t)(x))
#define __be64_to_cpu(x) ((uint64_t)(x))
#define __cpu_to_be32(x) ((uint32_t)(x))
#define __be32_to_cpu(x) ((uint32_t)(x)
#define __cpu_to_be16(x) ((uint16_t)(x))
#define __be16_to_cpu(x) ((uint16_t)(x))

#elif defined(__ARCH_LITTLE_ENDIAN)

#define __cpu_to_le64(x) ((uint64_t)(x))
#define __le64_to_cpu(x) ((uint64_t)(x))
#define __cpu_to_le32(x) ((uint32_t)(x))
#define __le32_to_cpu(x) ((uint32_t)(x))
#define __cpu_to_le16(x) ((uint16_t)(x))
#define __le16_to_cpu(x) ((uint16_t)(x))
#define __cpu_to_be64(x) ___constant_swab64((uint64_t)(x))
#define __be64_to_cpu(x) ___constant_swab64((uint64_t)(x))
#define __cpu_to_be32(x) ___constant_swab32((uint32_t)(x))
#define __be32_to_cpu(x) ___constant_swab32((uint32_t)(x))
#define __cpu_to_be16(x) ___constant_swab16((uint16_t)(x))
#define __be16_to_cpu(x) ___constant_swab16((uint16_t)(x))

#else

#error "please check your arch porting implementation for <asm/byteorder.h>"

#endif

/*
 * cpu_toxxyy 
 * 	xx : little endian or big endian
 * 	yy : bit to translate
 * .eg. cpu_to_le64 : translate host cpu 64bit(8byte) variable to little-endian 64bit(8byte) variable
 */
#define cpu_to_le64 __cpu_to_le64
#define le64_to_cpu __le64_to_cpu
#define cpu_to_le32 __cpu_to_le32
#define le32_to_cpu __le32_to_cpu
#define cpu_to_le16 __cpu_to_le16
#define le16_to_cpu __le16_to_cpu
#define cpu_to_be64 __cpu_to_be64
#define be64_to_cpu __be64_to_cpu
#define cpu_to_be32 __cpu_to_be32
#define be32_to_cpu __be32_to_cpu
#define cpu_to_be16 __cpu_to_be16
#define be16_to_cpu __be16_to_cpu


#endif /* __ACO_BYTE_ORDER_H */
