#ifndef __ACO_PRINTK_H
#define __ACO_PRINTK_H

#include <generated/autoconf.h>
#include <stdarg.h>

//porting implementation
#include <asm/printf.h>

/*
 * Desc : put 'ch' to stdout
 * Param:
 * 	     ch : the character to put out
 * Ret  :
 * 	     if success, return 'ch' self
 * 	     if erro , return the errno(<0)
 * Note : Need Arch Implemetation
 *        int __arch_put_char(int ch);
 */
static inline int putchar(int ch)
{
	return __arch_put_char(ch);
}

/*
 * Desc : put 'str' to stdout , and put '\n' to stdout
 * Param:
 *           str : the string to put out
 * Ret  :
 *           if success, return the size of 'str'(include the '\0')
 *           if erro , return the errno(<0)
 */
int puts(const char *str);

/*
 * Desc : format string 'format' and put it out to stdout
 * Param:
 *           format : the string to be formated
 *           ...    : other args
 * Ret  :
 *           if success, return the size of output character
 *           if erro , return the errno(<0)
 */
int printf(const char *format, ...);


#endif /* END __ACO_PRINTK_H */
