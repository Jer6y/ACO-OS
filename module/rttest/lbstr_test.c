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
						(*error)++;			\
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
	
	USE_FUNCTION(strncpy,NULL,"123",3);

        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strncpy,NULL,"123",20);

        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strncpy,NULL,"123",9);

        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strncpy,NULL,"123",(size_t)-1);

        CHECK_RESULT(NULL, judge_strcpy);

	buffer_sn[0] = 'a';
	buffer_sn[1] = 0;
	USE_FUNCTION(strncpy,buffer_sn,NULL,0);

        CHECK_RESULT("a", judge_strcpy);

        USE_FUNCTION(strncpy,buffer_sn,NULL,3);

        CHECK_RESULT("a", judge_strcpy);

        USE_FUNCTION(strncpy,buffer_sn,NULL,20);

        CHECK_RESULT("a", judge_strcpy);

        USE_FUNCTION(strncpy,buffer_sn,NULL,9);

        CHECK_RESULT("a", judge_strcpy);

        USE_FUNCTION(strncpy,buffer_sn,NULL,(size_t)-1);

        CHECK_RESULT("a", judge_strcpy);

	USE_FUNCTION(strncpy,NULL,NULL,0);

        CHECK_RESULT(NULL, judge_strcpy);

        USE_FUNCTION(strncpy,NULL,NULL,3);

        CHECK_RESULT(NULL, judge_strcpy);

        USE_FUNCTION(strncpy,NULL,NULL,20);

        CHECK_RESULT(NULL, judge_strcpy);

        USE_FUNCTION(strncpy,NULL,NULL,9);

        CHECK_RESULT(NULL, judge_strcpy);

        USE_FUNCTION(strncpy,NULL,NULL,(size_t)-1);

        CHECK_RESULT(NULL, judge_strcpy);

	USE_FUNCTION(strncpy,buffer_sn,"123",0);

        CHECK_RESULT("a", judge_strcpy);

        USE_FUNCTION(strncpy,buffer_sn,"123",3);

        CHECK_RESULT("123", judge_strcpy);

	USE_FUNCTION(strncpy,buffer_sn,"36",3);

        CHECK_RESULT("36", judge_strcpy);

	USE_FUNCTION(strncpy,buffer_sn,"5891",3);

        CHECK_RESULT("589", judge_strcpy);

        USE_FUNCTION(strncpy,buffer_sn,"123",20);

        CHECK_RESULT("123", judge_strcpy);

        USE_FUNCTION(strncpy,buffer_sn,"1234567890",9);

        CHECK_RESULT("123456789", judge_strcpy);

}

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

	return 0;
}
