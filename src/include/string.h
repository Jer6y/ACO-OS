#ifndef STRING_H
#define STRING_H
#include "def.h"

#define va_list(base) ((uint64*)(base)+1)
#define va_end(t)     ((t)= 0)
#define va_next(t,type) ( *(type *)((va*)((t)=(va*)(t)+1)-1))

#endif