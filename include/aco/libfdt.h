/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _INCLUDE_LIBFDT_H_
#define _INCLUDE_LIBFDT_H_

#include <generated/autoconf.h>

#include <aco/libfdt_env.h>
#include "../../scripts/dtc/libfdt/libfdt.h"

// porting implementation
// it is valid only when FDT_LIBRARY opened
#include <asm/fdt.h>

extern void* dtb_address;
#define DEVICE_TREE dtb_address

//__arch_dtb_get is porting implementation

static inline int fdt_init(void)
{
	void* __dtb = __arch_dtb_get();
	dtb_address = __dtb;
	return 0;
}

#endif /* _INCLUDE_LIBFDT_H_ */
