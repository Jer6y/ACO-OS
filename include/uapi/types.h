#ifndef __UAPI_TYPES_H
#define __UAPI_TYPES_H

#include <uapi/bits.h>

typedef	unsigned char		uint8;
typedef signed	 char		sint8;
typedef	unsigned short		uint16;
typedef signed	 short		sint16;
typedef unsigned int		uint32;
typedef signed 	 int		sint32;
typedef unsigned long long	uint64;
typedef signed 	 long long 	sint64;

#if (BITS32 == 1)
typedef unsigned long		uint32_l;
typedef signed 	 long		sint32_l;
#elif (BITS64 == 1)
typedef unsigned long		uint64_l;
typedef signed 	 long		sint64_l;
#else
  #error "unkown error!"
#endif

#endif
