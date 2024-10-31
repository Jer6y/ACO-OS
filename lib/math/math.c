#include <aco/math.h>

unsigned int JSHash(char *str, int size)
{
	unsigned int hash = 1315423911; // nearly a prime - 1315423911 = 3 * 438474637
	for(int i=0;i<size && str[i] !=0;i++)
		hash ^= ((hash << 5) + (*str++) + (hash >> 2));
	return (hash & 0x7FFFFFFF);
}

unsigned int SDBMHash(char *str, int size)
{
	unsigned int hash = 0;
	for(int i=0;i<size && str[i] !=0;i++)
		hash = (*str++) + (hash << 6) + (hash << 16) - hash;
	// equivalent to: hash = 65599*hash + (*str++);
	return (hash & 0x7FFFFFFF);
}
