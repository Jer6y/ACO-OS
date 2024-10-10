OUTPUT_ARCH(riscv)
ENTRY(_start)
#include <aco/link.h>

SECTIONS
{
  /*
   * ensure that entry.S / _entry is at 0x80000000,
   * where qemu's -kernel jumps.
   */
  . = 0x80200000;

  .text : {
    CODTEXT(1)
  }

  .rodata : {
    SRODATA(16)
    RODATA(16)    
  }

  .data : {
    SDATA(16)
    DATA(16)
  }

  .bss : {
    SBSS(16)
    BSS(16)
  }
}
