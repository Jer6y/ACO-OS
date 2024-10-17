
#include <aco/types.h>
#include <aco/init.h>
#include <aco/printf.h>

extern int rt_libstr(int* success, int* error);
extern int rt_init_section(int* success, int* error);
int b = 10;
int* c = &b;
void main(void)
{
	int *a = &b;
	printf("address b through c : %p\n", c);
	printf("address b through a : %p\n", a);
	int suc = 0;
	int err = 0;
	rt_libstr(&suc,&err);
	rt_init_section(&suc,&err);
	return;
}

