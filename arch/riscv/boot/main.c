
#include <aco/types.h>
#include <aco/init.h>
#include <aco/printf.h>
#include <aco/log.h>

extern int rt_libstr(int* success, int* error);
extern int rt_init_section(int* success, int* error);
extern int log_module_init(void);
extern int rt_list_api(int* success, int* error);
int b = 10;
int* c = &b;
void main(void)
{
	int *a = &b;
	printf("address b through c : %p\n", c);
	printf("address b through a : %p\n", a);

	return;
}

