/* SPDX-License-Identifier: GPL-1.0 */
#ifndef __ACO_LIMITS_H
#define __ACO_LIMITS_H

#include <aco/types.h>

#define NR_OPEN         1024

#define NGROUPS_MAX    65536    /* supplemental group IDs are available */
#define ARG_MAX       131072    /* # bytes of args + environ for exec() */
#define LINK_MAX         127    /* # links a file may have */
#define MAX_CANON        255    /* size of the canonical input queue */
#define MAX_INPUT        255    /* size of the type-ahead buffer */
#define NAME_MAX         255    /* # chars in a file name */
#define PATH_MAX        4096    /* # chars in a path name including nul */
#define PIPE_BUF        4096    /* # bytes in atomic write to a pipe */
#define XATTR_NAME_MAX   255    /* # chars in an extended attribute name */
#define XATTR_SIZE_MAX 65536    /* size of an extended attribute value (64k) */
#define XATTR_LIST_MAX 65536    /* size of extended attribute namelist (64k) */
#define RTSIG_MAX         32

#ifndef SIZE_MAX
#define SIZE_MAX        (~(size_t)0)
#endif 

#ifndef PHYS_ADDR_MAX
#define PHYS_ADDR_MAX   (~(phyaddr_t)0)
#endif

#define U8_MAX          ((uint8_t)~0U)
#define S8_MAX          ((signed char)(U8_MAX >> 1))
#define S8_MIN          ((signed char)(-S8_MAX - 1))
#define U16_MAX         ((uint16_t)~0U)
#define S16_MAX         ((signed short)(U16_MAX >> 1))
#define S16_MIN         ((signed short)(-S16_MAX - 1))
#define U32_MAX         ((uint32_t)~0U)
#define U32_MIN         ((uint32_t)0)
#define S32_MAX         ((signed int)(U32_MAX >> 1))
#define S32_MIN         ((signed int)(-S32_MAX - 1))
#define U64_MAX         ((uint64_t)~0ULL)
#define S64_MAX         ((signed long long)(U64_MAX >> 1))
#define S64_MIN         ((signed long long)(-S64_MAX - 1))

#endif /* __ACO_LIMITS_H */

