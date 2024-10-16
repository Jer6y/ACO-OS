
#include <asm/printf.h>

int b = 10;
int *c = &b;
void main(void)
{
	asm volatile ("add zero,zero,zero");
	riscv_printf("bef_b : %d\n",b);
	asm volatile ("add zero,zero,zero");
	int*p = &b;
	asm volatile ("add zero,zero,zero");
	riscv_printf("test printf %p\n",p);
	asm volatile ("add zero,zero,zero");
	b = 20;
	asm volatile ("add zero,zero,zero");
	riscv_printf("aft_b : %d\n",b);
	asm volatile ("add zero,zero,zero");
	riscv_printf("c : %p\n",c);
	asm volatile ("add zero,zero,zero");
}

