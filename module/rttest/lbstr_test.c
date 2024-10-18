#include "rttest.h"
#include <aco/string.h>
#include <aco/types.h>


#define DEFINE_RESULT_VAL(type)	type ret;

#define USE_FUNCTION(func,...)	ret = func(__VA_ARGS__);

#define CHECK_RESULT(predict, judge_func)	\
				do						\
				{						\
					int res = judge_func(ret, predict);	\
					if(res == 1)				\
						(*success)++;			\
					else					\
					{					\
						(*error)++;			\
					}					\
				} while(0)

FUNC_BUILTIN int judge_strlen(int result, int judge_baseline)
{
	return result == judge_baseline;
}

FUNC_BUILTIN void test_strlen(int* success,int* error)
{
	DEFINE_RESULT_VAL(int);

	USE_FUNCTION(strlen,NULL);
	
	CHECK_RESULT(0, judge_strlen);

	USE_FUNCTION(strlen,"");

	CHECK_RESULT(0, judge_strlen);

	USE_FUNCTION(strlen,"23412");

	CHECK_RESULT(5, judge_strlen);

	USE_FUNCTION(strlen,"2_ c	0");

	CHECK_RESULT(6, judge_strlen);

	USE_FUNCTION(strlen,"\n");

	CHECK_RESULT(1, judge_strlen);

	USE_FUNCTION(strlen,"2\n");

	CHECK_RESULT(2, judge_strlen);
}

FUNC_BUILTIN int judge_strcpy(const char* result, const char* judge_baseline)
{
	if(judge_baseline == NULL)
	{
		if(result == NULL)
			return 1;
		return 0;
	}
	if(result == NULL)
		return 0;
	int i =0;
	while(1)
	{
		if(result[i] == 0 || judge_baseline[i] == 0)
		{
			if(result[i] ==0 && judge_baseline[i] == 0)
				return 1;
			return 0;
		}
		if(result[i] != judge_baseline[i])
			return 0;
		i++;
	}
	return 0;
}


FUNC_BUILTIN void test_strcpy_safe(int* success,int* error)
{
        static char buffer_ss[10];
        DEFINE_RESULT_VAL(char*);

        USE_FUNCTION(strcpy_safe,NULL,"123",10);

        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strcpy_safe,NULL,"123",0);

        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strcpy_safe,NULL,"123",(size_t)-1);

        CHECK_RESULT(NULL, judge_strcpy);

        buffer_ss[0] = 'a';
        buffer_ss[1] = '\0';
        USE_FUNCTION(strcpy_safe,buffer_ss,NULL,10);

        CHECK_RESULT("a", judge_strcpy);

	USE_FUNCTION(strcpy_safe,NULL,NULL,0);

        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strcpy_safe,NULL,NULL,100);

        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strcpy_safe,NULL,NULL,(size_t)-1);

        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strcpy_safe,buffer_ss,"",10);

        CHECK_RESULT("", judge_strcpy);

	USE_FUNCTION(strcpy_safe,buffer_ss,"\n",10);

        CHECK_RESULT("\n", judge_strcpy);

	USE_FUNCTION(strcpy_safe,buffer_ss,"234",10);

        CHECK_RESULT("234", judge_strcpy);

	USE_FUNCTION(strcpy_safe,buffer_ss,"123456789",10);

        CHECK_RESULT("123456789", judge_strcpy);

	USE_FUNCTION(strcpy_safe,buffer_ss,"1234567890",10);

        CHECK_RESULT("123456789", judge_strcpy);

	USE_FUNCTION(strcpy_safe,buffer_ss,"1234567890abc",10);

        CHECK_RESULT("123456789", judge_strcpy);

	USE_FUNCTION(strcpy_safe,buffer_ss,"123",10);

        CHECK_RESULT("123", judge_strcpy);

	USE_FUNCTION(strcpy_safe,buffer_ss,"1237982",0);

        CHECK_RESULT("123", judge_strcpy);
}

FUNC_BUILTIN void test_strcpy(int* success,int* error)
{
	static char buffer[10];
	DEFINE_RESULT_VAL(char*);

        USE_FUNCTION(strcpy,NULL,"123");

        CHECK_RESULT(NULL, judge_strcpy);

	buffer[0] = 'a';
	buffer[1] = '\0';
        USE_FUNCTION(strcpy,buffer,NULL);

        CHECK_RESULT("a", judge_strcpy);

	USE_FUNCTION(strcpy,NULL,NULL);

        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strcpy,buffer,"");

        CHECK_RESULT("", judge_strcpy);

	USE_FUNCTION(strcpy,buffer,"\n");

        CHECK_RESULT("\n", judge_strcpy);

	USE_FUNCTION(strcpy,buffer,"123");

        CHECK_RESULT("123", judge_strcpy);

	USE_FUNCTION(strcpy,buffer,"123456789");

        CHECK_RESULT("123456789", judge_strcpy);

	USE_FUNCTION(strcpy,buffer,"12");

        CHECK_RESULT("12", judge_strcpy);
}

FUNC_BUILTIN void test_strncpy(int* success,int* error)
{
	static char buffer_sn[10];

	DEFINE_RESULT_VAL(char*);

        USE_FUNCTION(strncpy,NULL,"123",0);
        CHECK_RESULT(NULL, judge_strcpy);
	
	USE_FUNCTION(strncpy_safe,NULL,"123",0,0);
        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strncpy,NULL,"123",3);
        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strncpy_safe,NULL,"123",3,3);
        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strncpy,NULL,"123",20);
        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strncpy_safe,NULL,"123",20,(size_t)-1);
        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strncpy,NULL,"123",9);
        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strncpy_safe,NULL,"123",9,3);
        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strncpy,NULL,"123",(size_t)-1);
        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strncpy_safe,NULL,"123",(size_t)-1,30);
        CHECK_RESULT(NULL, judge_strcpy);

	buffer_sn[0] = 'a';
	buffer_sn[1] = 0;

	USE_FUNCTION(strncpy,buffer_sn,NULL,0);
        CHECK_RESULT("a", judge_strcpy);

	USE_FUNCTION(strncpy_safe,buffer_sn,NULL,0,0);
        CHECK_RESULT("a", judge_strcpy);

        USE_FUNCTION(strncpy,buffer_sn,NULL,3);
        CHECK_RESULT("a", judge_strcpy);

	USE_FUNCTION(strncpy_safe,buffer_sn,NULL,3,3);
        CHECK_RESULT("a", judge_strcpy);

        USE_FUNCTION(strncpy,buffer_sn,NULL,20);
        CHECK_RESULT("a", judge_strcpy);

	USE_FUNCTION(strncpy_safe,buffer_sn,NULL,20,20);
        CHECK_RESULT("a", judge_strcpy);

        USE_FUNCTION(strncpy,buffer_sn,NULL,9);
        CHECK_RESULT("a", judge_strcpy);

	USE_FUNCTION(strncpy_safe,buffer_sn,NULL,9,9);
        CHECK_RESULT("a", judge_strcpy);

        USE_FUNCTION(strncpy,buffer_sn,NULL,(size_t)-1);
        CHECK_RESULT("a", judge_strcpy);

	USE_FUNCTION(strncpy_safe,buffer_sn,NULL,(size_t)-1,(size_t)-1);
        CHECK_RESULT("a", judge_strcpy);

	USE_FUNCTION(strncpy,NULL,NULL,0);
        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strncpy_safe,NULL,NULL,0,0);
        CHECK_RESULT(NULL, judge_strcpy);

        USE_FUNCTION(strncpy,NULL,NULL,3);
        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strncpy_safe,NULL,NULL,3,3);
        CHECK_RESULT(NULL, judge_strcpy);

        USE_FUNCTION(strncpy,NULL,NULL,20);
        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strncpy_safe,NULL,NULL,20,20);
        CHECK_RESULT(NULL, judge_strcpy);

        USE_FUNCTION(strncpy,NULL,NULL,9);
        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strncpy_safe,NULL,NULL,9,9);
        CHECK_RESULT(NULL, judge_strcpy);

        USE_FUNCTION(strncpy,NULL,NULL,(size_t)-1);
        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strncpy_safe,NULL,NULL,(size_t)-1,(size_t)-1);
        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strncpy,buffer_sn,"123",0);
        CHECK_RESULT("a", judge_strcpy);
	
	USE_FUNCTION(strncpy_safe,buffer_sn,"123",0,10);
        CHECK_RESULT("a", judge_strcpy);
	
	USE_FUNCTION(strncpy_safe,buffer_sn,"123",0,3);
        CHECK_RESULT("a", judge_strcpy);

	USE_FUNCTION(strncpy_safe,buffer_sn,"123",0,0);
        CHECK_RESULT("a", judge_strcpy);

	USE_FUNCTION(strncpy_safe,buffer_sn,"123",0,(size_t)-1);
        CHECK_RESULT("a", judge_strcpy);

        USE_FUNCTION(strncpy,buffer_sn,"123",3);
        CHECK_RESULT("123", judge_strcpy);

	USE_FUNCTION(strncpy_safe,buffer_sn,"123",3,10);
        CHECK_RESULT("123", judge_strcpy);

	USE_FUNCTION(strncpy_safe,buffer_sn,"123",3,0);
        CHECK_RESULT("123", judge_strcpy);

	USE_FUNCTION(strncpy_safe,buffer_sn,"123",3,2);
        CHECK_RESULT("1", judge_strcpy);

	USE_FUNCTION(strncpy,buffer_sn,"36",3);
        CHECK_RESULT("36", judge_strcpy);

	USE_FUNCTION(strncpy_safe,buffer_sn,"36",3,10);
        CHECK_RESULT("36", judge_strcpy);

	USE_FUNCTION(strncpy_safe,buffer_sn,"36",3,0);
        CHECK_RESULT("36", judge_strcpy);

	USE_FUNCTION(strncpy_safe,buffer_sn,"36",3,(size_t)-1);
        CHECK_RESULT("36", judge_strcpy);

	USE_FUNCTION(strncpy_safe,buffer_sn,"36",3,1);
        CHECK_RESULT("", judge_strcpy);

	USE_FUNCTION(strncpy_safe,buffer_sn,"36",3,2);
        CHECK_RESULT("3", judge_strcpy);

	USE_FUNCTION(strncpy,buffer_sn,"5891",3);
        CHECK_RESULT("589", judge_strcpy);

	USE_FUNCTION(strncpy_safe,buffer_sn,"5891",3,1);
        CHECK_RESULT("", judge_strcpy);

	USE_FUNCTION(strncpy_safe,buffer_sn,"5891",3,10);
        CHECK_RESULT("589", judge_strcpy);

	USE_FUNCTION(strncpy_safe,buffer_sn,"5891",3,0);
        CHECK_RESULT("589", judge_strcpy);

	USE_FUNCTION(strncpy_safe,buffer_sn,"5891",3,3);
        CHECK_RESULT("58", judge_strcpy);

	USE_FUNCTION(strncpy_safe,buffer_sn,"5891",3,(size_t)-1);
        CHECK_RESULT("589", judge_strcpy);

        USE_FUNCTION(strncpy,buffer_sn,"123",20);

        CHECK_RESULT("123", judge_strcpy);

        USE_FUNCTION(strncpy,buffer_sn,"1234567890",9);

        CHECK_RESULT("123456789", judge_strcpy);
}


FUNC_BUILTIN void test_strcmp(int* success,int* error)
{
        DEFINE_RESULT_VAL(int);

        USE_FUNCTION(strcmp,NULL,"123");
        CHECK_RESULT(-'1', judge_strlen);

	USE_FUNCTION(strcmp,"123",NULL);
        CHECK_RESULT('1', judge_strlen);

	USE_FUNCTION(strcmp,NULL,NULL);
        CHECK_RESULT(0, judge_strlen);

	USE_FUNCTION(strcmp,"123","123");
        CHECK_RESULT(0, judge_strlen);

	USE_FUNCTION(strcmp,"123","12");
        CHECK_RESULT('3', judge_strlen);

	USE_FUNCTION(strcmp,"12","123");
        CHECK_RESULT(-'3', judge_strlen);

	USE_FUNCTION(strcmp,"12345","23");
        CHECK_RESULT('1' - '2', judge_strlen);

	USE_FUNCTION(strcmp,"12345","1123455");
        CHECK_RESULT('2' - '1', judge_strlen);

	USE_FUNCTION(strcmp,"12345","12345");
        CHECK_RESULT(0, judge_strlen);
}

FUNC_BUILTIN void test_strcat(int* success,int* error)
{
        static char buffer_cat[10];

        DEFINE_RESULT_VAL(char*);

        USE_FUNCTION(strcat,NULL,"123");
        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strcat,"123",NULL);
        CHECK_RESULT("123", judge_strcpy);

	USE_FUNCTION(strcat,NULL,NULL);
        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strcat,buffer_cat,"12");
        CHECK_RESULT("12", judge_strcpy);

	USE_FUNCTION(strcat,buffer_cat,"345");
        CHECK_RESULT("12345", judge_strcpy);

	USE_FUNCTION(strcat,buffer_cat,NULL);
        CHECK_RESULT("12345", judge_strcpy);

	USE_FUNCTION(strcat,buffer_cat,"6789");
        CHECK_RESULT("123456789", judge_strcpy);
}


FUNC_BUILTIN int judge_strstr(char* result, char* judge_baseline)
{
        return result == judge_baseline;
}


FUNC_BUILTIN void test_strchr(int* success,int* error)
{
        DEFINE_RESULT_VAL(char*);
	static char buffer_strstr[6];
	buffer_strstr[0] = '1';
	buffer_strstr[1] = '2';
	buffer_strstr[2] = '1';
	buffer_strstr[3] = '2';
	buffer_strstr[4] = '1';
	buffer_strstr[5] = 0;
        USE_FUNCTION(strchr,NULL,'a');
        CHECK_RESULT(NULL, judge_strstr);
	
	USE_FUNCTION(strrchr,NULL,'a');
        CHECK_RESULT(NULL, judge_strstr);
	
	USE_FUNCTION(strchr,buffer_strstr,'\0');
        CHECK_RESULT(NULL, judge_strstr);
        
	USE_FUNCTION(strrchr,buffer_strstr,'\0');
        CHECK_RESULT(NULL, judge_strstr);
	
	USE_FUNCTION(strchr,buffer_strstr,'1');
        CHECK_RESULT(buffer_strstr + 0, judge_strstr);
        
	USE_FUNCTION(strrchr,buffer_strstr,'1');
        CHECK_RESULT(buffer_strstr + 4, judge_strstr);
	
	USE_FUNCTION(strchr,buffer_strstr,'2');
        CHECK_RESULT(buffer_strstr + 1, judge_strstr);

        USE_FUNCTION(strrchr,buffer_strstr,'2');
        CHECK_RESULT(buffer_strstr + 3, judge_strstr);
}

FUNC_BUILTIN void test_memcmp(int* success,int* error)
{
        DEFINE_RESULT_VAL(int);
	
	USE_FUNCTION(memcmp,NULL,NULL,0);
        CHECK_RESULT(0, judge_strlen);

	USE_FUNCTION(memcmp,NULL,NULL,10);
        CHECK_RESULT(0, judge_strlen);

	USE_FUNCTION(memcmp,NULL,NULL,(size_t)-1);
        CHECK_RESULT(0, judge_strlen);

	USE_FUNCTION(memcmp,(void*)"123",NULL,0);
        CHECK_RESULT(0, judge_strlen);

	USE_FUNCTION(memcmp,(void*)"123",NULL,3);
        CHECK_RESULT('1', judge_strlen);

	USE_FUNCTION(memcmp,NULL,(void*)"231",3);
        CHECK_RESULT(-'2', judge_strlen);

	USE_FUNCTION(memcmp,NULL,(void*)"231",0);
        CHECK_RESULT(0, judge_strlen);

	USE_FUNCTION(memcmp,(void*)"231",(void*)"231",3);
        CHECK_RESULT(0, judge_strlen);

	USE_FUNCTION(memcmp,(void*)"23",(void*)"231",1);
        CHECK_RESULT(0, judge_strlen);
	
	USE_FUNCTION(memcmp,(void*)"23",(void*)"231",3);
        CHECK_RESULT(-'1', judge_strlen);
}

static char mem_ops_data[128];

FUNC_BUILTIN int judge_memset(char* result, char* baseline)
{
	return result == baseline;
}

FUNC_BUILTIN void test_memset(int* success,int* error)
{
        DEFINE_RESULT_VAL(char*);

        USE_FUNCTION(memset,NULL,'a',0);
        CHECK_RESULT(NULL, judge_memset);

	USE_FUNCTION(memset,NULL,'a',10);
        CHECK_RESULT(NULL, judge_memset);
	
	USE_FUNCTION(memset,NULL,'a',(size_t)-1);
        CHECK_RESULT(NULL, judge_memset);

	USE_FUNCTION(memset,mem_ops_data,'a',0);
        CHECK_RESULT(mem_ops_data, judge_memset);

	USE_FUNCTION(memset,mem_ops_data,'a',32);
	int i;
	for(i=0;i<32;i++)
	{
		if(mem_ops_data[i] != 'a')
		{
			(*error)++;
			break;
		}
	}
	if(i>=32)
		(*success)++;

	USE_FUNCTION(memset,mem_ops_data,'b',16);
        for(i=0;i<32;i++)
        {
		if(i<=15)
		{
			if(mem_ops_data[i] != 'b')
			{
                                (*error)++;
				break;
			}
		}
		else
		{
                	if(mem_ops_data[i] != 'a')
			{
                        	(*error)++;
				break;
			}
		}
        }
        if(i>=32)
                (*success)++;

	USE_FUNCTION(memset,mem_ops_data,'b',128);
        for(i=0;i<128;i++)
        {
                if(mem_ops_data[i] != 'b')
		{
                        (*error)++;
			break;
        	}
	}
        if(i>=128)
                (*success)++;
}

/*
void *  memcpy(void *des, const void *src, size_t n);

void *  memchr(const void *str, int c, size_t n);
*/

int rt_libstr(int* success, int* error)
{
	int suc = 0;
	int err = 0;
	test_strlen(&suc,&err);
	rttest_printf("[libstr] : strlen test %d/%d\n", suc, suc+err);
	(*success) += suc;
	(*error)   += err;
	suc = 0;
	err = 0;
	test_strcpy(&suc,&err);
	rttest_printf("[libstr] : strcpy test %d/%d\n", suc, suc+err);
        (*success) += suc;
        (*error)   += err;
	suc = 0;
	err = 0;
	test_strcpy_safe(&suc,&err);
	rttest_printf("[libstr] : strcpy_safe %d/%d\n", suc, suc+err);
        (*success) += suc;
        (*error)   += err;
	suc = 0;
	err = 0;
	test_strncpy(&suc,&err);
	rttest_printf("[libstr] : strncpy tst %d/%d\n", suc, suc+err);
        (*success) += suc;
        (*error)   += err;
	suc = 0;
	err = 0;
	test_strcmp(&suc,&err);
	rttest_printf("[libstr] : strcmp test %d/%d\n", suc, suc+err);
        (*success) += suc;
        (*error)   += err;
	suc = 0;
	err = 0;
	test_strcat(&suc,&err);
	rttest_printf("[libstr] : strcat test %d/%d\n", suc, suc+err);
        (*success) += suc;
        (*error)   += err;
	suc = 0;
	err = 0;
	test_strchr(&suc,&err);
	rttest_printf("[libstr] : strstr test %d/%d\n", suc, suc+err);
        (*success) += suc;
        (*error)   += err;
	suc = 0;
	err = 0;
	test_memcmp(&suc,&err);
	rttest_printf("[libstr] : memcmp test %d/%d\n", suc, suc+err);
        (*success) += suc;
        (*error)   += err;
	suc = 0;
	err = 0;
	test_memset(&suc,&err);
	rttest_printf("[libstr] : memset test %d/%d\n", suc, suc+err);
        (*success) += suc;
        (*error)   += err;
	return 0;
}
