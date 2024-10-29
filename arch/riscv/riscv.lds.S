OUTPUT_ARCH(riscv)
ENTRY(_start)
#include <aco/link.h>

SECTIONS
{
    . = KERNEL_OFFSET;
    LINK_ALIGN(PAGE_ALIGN)
    KERNEL_START
    SECTION_HEAD

    SECTION_TEXT
    
    /* this align is to
     * seperate text and
     * data. this is helpful
     * when you set mmu
     * before this, set RX
     * after this,  set RW
     */
    LINK_ALIGN(PAGE_ALIGN)
    _seperate_line = .;
    SECTION_DATA
    
    SECTION_PERCPU

    SECTION_INIT

    SECTION_BSS

    SECTIONS_ARCH_SPECIFIC
    KERNEL_END
}
