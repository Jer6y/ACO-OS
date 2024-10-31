/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _INCLUDE_LIBFDT_H_
#define _INCLUDE_LIBFDT_H_

#include <generated/autoconf.h>

#include <aco/libfdt_env.h>
#include "../../scripts/dtc/libfdt/libfdt.h"

extern void* dtb_address;
#define DEVICE_TREE dtb_address

static inline int fdt_init(uintptr_t dtb)
{
	dtb_address = (void*)dtb;
	return 0;
}

#endif /* _INCLUDE_LIBFDT_H_ */
