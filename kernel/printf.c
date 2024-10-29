#include <aco/printf.h>
#include <aco/string.h>
#include <aco/vsnprintf.h>

int puts(const char *str)
{
	int len = strlen(str);
	for(int i=0;i<len;i++)
	{
		putchar((int)str[i]);
	}
	putchar((int)'\n');
	return (len+1);
}

int printf(const char *format, ...)
{
	char buffer[256];
	va_list arg;
	va_start(arg, format);
	int done = vsnprintf(buffer, 512, format, arg);  // 将格式化的内容(字符串)保存在buffer
	va_end(arg);
	int len = strlen(buffer);
	for(int i=0;i<len;i++)
		putchar((int)buffer[i]);
	return done;
}
