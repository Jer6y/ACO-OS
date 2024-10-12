#ifndef __ASM_ATOMIC_H
#define __ASM_ATOMIC_H

#ifndef __GNUC__

/* if we don't use gcc , please implement interface that atomic.h provide*/
/* we now no support */

#error "riscv : no support atomic api when gcc is not used"

#endif /* __GNUC__ */

#endif /* __ASM_ATOMIC_H */
