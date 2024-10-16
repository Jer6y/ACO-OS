#ifndef __ACO_STRING_H_
#define __ACO_STRING_H_

#include <aco/types.h>
/*
 * Include machine specific inline routines
 */
#include <asm/string.h>

size_t  strlen(const char * src) ;

char *	strcpy(char *dest, const char *src);

char *	strncpy(char *dest, const char *src, size_t n);

int 	strcmp(const char* src,const char *des);

char *	strcat(char *dest, const char *src);

char *	strchr(const char *str, int c);

char *	strrchr(const char *str, int c);

int 	memcmp(const void *str1, const void *str2, size_t n);

void *	memcpy(void *des, const void *src, size_t n);

void *	memchr(const void *str, int c, size_t n);

void *	memset(const void *des, uint8 c,size_t n);

int 	sprintf(char *buf, const char *fmt, ...);

int 	vsprintf(char *buf, const char *fmt, va_list args);

#endif /*__ACO_STRING_H_ */
