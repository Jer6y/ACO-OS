
#include <aco/types.h>
#include <aco/init.h>
#include <aco/printf.h>

int b = 10;
int* c = &b;
void main(void)
{
	int * a = &b;
	printf("address b through c : %p\n", c);
	printf("address b through a : %p\n", a);
}

