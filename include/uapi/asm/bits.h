#ifndef __UAPI_ASM_BITS_H
#define __UAPI_ASM_BITS_H

#ifndef __riscv
  #error "[ERROR] : please check your gnu toolchain architecture"
#endif

#if __riscv_xlen == 32
  #define __arch_is_bit32 1
  #define __arch_is_bit64 0
#elif __riscv_xlen == 64
  #define __arch_is_bit32 0
  #define __arch_is_bit64 1
#else
  #error "[ERROR] : unkown error !!"
#endif

#endif /* __UAPI_ASM_BITS_H */
