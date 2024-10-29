#ifndef __ACO_ASM_ASSERT_H
#define __ACO_ASM_ASSERT_H

#include <csr.h>
static inline void riscv_assertion(const char* error_condition)
{
        intr_off(); //disable local interrupt
        while(1)
                asm volatile("wfi");
        //just spinself
}


#define __arch_assert_error(x)  riscv_assertion(x)

#endif /* __ACO_ASM_ASSERT_H */
