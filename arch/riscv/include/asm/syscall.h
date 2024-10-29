#ifndef __ACO_ASM_SYSCALL
#define __ACO_ASM_SYSCALL

#include <aco/bits.h>

#if (BITS32 == 1)
  #include <asm/syscall_32.h>
#elif (BITS64 == 1)
  #include <asm/syscall_64.h>
#else
  #error "unkown error!"
#endif

#endif
