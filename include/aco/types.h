#ifndef __ACO_TYPES_H
#define __ACO_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include <aco/bits.h>

#define  STATIC static
#define  INLINE inline
#define  FUNC_BUILTIN STATIC INLINE
#define  PRIVATE_VAR  STATIC

typedef  uintptr_t  		 phyaddr_t;
typedef  uintptr_t  		 viraddr_t;
typedef  unsigned char           uint8;
typedef  signed   char           sint8;
typedef  unsigned short          uint16;
typedef  signed   short          sint16;
typedef  unsigned int            uint32;
typedef  signed   int            sint32;
typedef  unsigned long long      uint64;
typedef  signed   long long      sint64;

//extend types to use
#include <asm/types.h>

#endif
