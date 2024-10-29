#ifndef __ACO_STRING_H_
#define __ACO_STRING_H_

#include <aco/types.h>

size_t  strlen(const char* src) ;

size_t  strnlen(const char* s, size_t maxlen);

char *	strcpy(char *dest, const char *src);

char*   strcpy_safe(char *dest, const char *src, size_t max_size);

char *	strncpy(char *dest, const char *src, size_t n);

char*   strncpy_safe(char *dest, const char *src, size_t n, size_t max_size);

int 	strcmp(const char* src,const char *des);

char *	strcat(char *dest, const char *src);

char *	strchr(const char *str, int c);

char *	strrchr(const char *str, int c);

int 	memcmp(const void *str1, const void *str2, size_t n);

void *	memcpy(void *des, const void *src, size_t n);

void *	memchr(const void *str, int c, size_t n);

void *	memset(const void *des, uint8 c,size_t n);

void *  memmove(void *str1, const void *str2, size_t n);

int 	sprintf(char *buf, const char *fmt, ...);

int 	vsprintf(char *buf, const char *fmt, va_list args);


#include <aco/vsnprintf.h>

#endif /*__ACO_STRING_H_ */
