#include "rttest.h"
#include <aco/types.h>

static uint8 dt_bss_test[13];
static uint8 dt_data_test[7] = { 0x34,0x61,0x78,0x12,0x47,0x98,0x17 };

int rt_init_section(int* success, int* error)
{
	uint8 pass_bss  = 1;
	uint8 pass_data = 0;
	for(int i =0;i<13;i++)
	{
		if(dt_bss_test[i] != 0)
			pass_bss = 0;
	}

	if(	dt_data_test[0] == 0x34 	&&\
	  	dt_data_test[1]	== 0x61		&&\
	  	dt_data_test[2]	== 0x78		&&\
	  	dt_data_test[3]	== 0x12		&&\
	  	dt_data_test[4]	== 0x47		&&\
	  	dt_data_test[5]	== 0x98		&&\
	  	dt_data_test[6]	== 0x17	)
	{
		pass_data = 1;
	}

	if(pass_bss)
	{
		rttest_printf("[init_section] : pass bss test 1/1\n");
		(*success)++;
	}
	else
	{
		rttest_printf("[init_section] : bss failed 0/1\n");
		(*error)++;
	}
	if(pass_data)
	{
		rttest_printf("[init_section] : pass data test 1/1\n");
		(*success)++;
	}
	else
	{
		rttest_printf("[init_section] : data failed 0/1\n");
		(*error)++;
	}
	return 0;
}
