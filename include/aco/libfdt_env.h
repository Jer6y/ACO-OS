/* SPDX-License-Identifier: GPL-2.0 */
#ifndef LIBFDT_ENV_H
#define LIBFDT_ENV_H

#include <aco/limits.h>	/* For INT_MAX */
#include <aco/string.h>
#include <aco/byteorder.h>

#ifndef INT32_MAX
#define INT32_MAX	S32_MAX
#endif

#ifndef UINT32_MAX
#define UINT32_MAX	U32_MAX
#endif 

#ifndef INT_MAX
#define INT_MAX		INT32_MAX
#endif

typedef uint16_t fdt16_t;
typedef uint32_t fdt32_t;
typedef uint64_t fdt64_t;

#define fdt32_to_cpu(x) be32_to_cpu(x)
#define cpu_to_fdt32(x) cpu_to_be32(x)
#define fdt64_to_cpu(x) be64_to_cpu(x)
#define cpu_to_fdt64(x) cpu_to_be64(x)

#endif /* LIBFDT_ENV_H */
