/*
 * assert 断言
 * used to debug
 * 依赖于 arch_port , arch库 
 * 以及一些dbg 用的函数
 * 
 */

#ifndef ASSERT_H
#define ASSERT_H

#define ASSERT(x) if(!(x)) { assert_failed(__FILE__, __LINE__); }


void assert_failed(char* file, int line);

void panic(char *str, ...);

#endif