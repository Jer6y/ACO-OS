#ifndef __ACO_BITS_H
#define __ACO_BITS_H

//arch porting implementation
#include <asm/bits.h>

//this file is to tell other file
//what bit width for the specific platform
/**************************define for BITS32****************************************/
#ifdef BITS32
#undef BITS32
#endif

#define BITS32 __arch_is_bit32


/**************************define for BITS64****************************************/
#ifdef BITS64
#undef BITS64
#endif
#define BITS64 __arch_is_bit64

#define IS_BIT32 (BITS32 == 1)
#define IS_BIT64 (BITS64 == 1)


#endif
