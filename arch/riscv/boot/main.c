
#include <aco/types.h>
#include <aco/init.h>
#include <aco/printf.h>

int b = 10;
int* c = &b;
void main(void)
{
	int *a = &b;
	printf("address b through c : %p\n", c);
	printf("address b through a : %p\n", a);
	printf("test for printf %d,%f,%.2lf,%s,%c,%o\n",123,2.333333,2.333333,".2f",'b',0723);
	return;
}

