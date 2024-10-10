#ifndef __UAPI_ASM_SYSCALL
#define __UAPI_ASM_SYSCALL

#include <uapi/bits.h>

#if (BITS32 == 1)
  #include <uapi/asm/syscall_32.h>
#elif (BITS64 == 1)
  #include <uapi/asm/syscall_64.h>
#else
  #error "unkown error!"
#endif

#endif
