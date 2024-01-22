/*
 * 字符串公共库
 * 只依赖Los_type库
 * 
 */
#ifndef STRING_H
#define STRING_H

typedef char* va_list;

#define va_start(vlist,addrs_arg)   ((vlist) = (va_list)addrs_arg)

#define va_arg(args,type)   (*(type*)((va_list*)(args=(va_list)((va_list *)(args)+1))-1))

#define va_end(vlist) ((vlist) = (va_list)0)

size_t  strlen(const char * src);

char *strcpy(char *dest, const char *src);

char *strncpy(char *dest, const char *src, size_t n);

int strcmp(const char* src,const char *des);

char *strcat(char *dest, const char *src);

char *strchr(const char *str, int c);

char *strrchr(const char *str, int c);

int memcmp(const void *str1, const void *str2, size_t n);

void *memcpy(void *des, const void *src, size_t n);

void *memchr(const void *str, int c, size_t n);

void *memset(const void *des, uint8 c,size_t n);

int sprintf(char *buf, const char *fmt, ...);

int printf(const char* str,...);

int vsprintf(char *buf, const char *fmt, va_list args);

#endif