#ifndef __ACO_ATOMIC_H
#define __ACO_ATOMIC_H

typedef unsigned int atomic_t;


#define ATOMIC_INIT_VAL(init_val)	((atomic_t)(init_val))

#define DEFINE_ATOMIC(var_name, init_val)	static atomic_t var_name = ATOMIC_INIT_VAL(init_val)


// if we use gcc , use gcc built-in atomic operation
// else take it to specific arch to implement it
#ifdef __GNUC__

static inline void  atomic_init(atomic_t* ptr, unsigned int value)
{
	(*ptr) = value;
}

static inline atomic_t atomic_fetch_and_add(atomic_t* ptr, atomic_t value)
{
	return __sync_fetch_and_add(ptr, value);
}

static inline atomic_t atomic_fetch_and_sub(atomic_t* ptr, atomic_t value)
{
	return __sync_fetch_and_sub(ptr, value);
}

static inline atomic_t atomic_fetch_and_or(atomic_t* ptr, atomic_t value)
{
	return __sync_fetch_and_or(ptr, value);
}

static inline atomic_t atomic_fetch_and_and(atomic_t* ptr, atomic_t value)
{
	return __sync_fetch_and_and(ptr, value);
}

static inline atomic_t atomic_fetch_and_xor(atomic_t* ptr, atomic_t value)
{
	return __sync_fetch_and_xor(ptr, value);
}

static inline atomic_t atomic_fetch_and_nand(atomic_t* ptr, atomic_t value)
{
	return __sync_fetch_and_nand(ptr, value);
}

static inline atomic_t atomic_add_and_fetch(atomic_t* ptr, atomic_t value)
{
	return __sync_add_and_fetch(ptr, value);
}

static inline atomic_t atomic_sub_and_fetch(atomic_t* ptr, atomic_t value)
{
	return __sync_sub_and_fetch(ptr, value);
}

static inline atomic_t atomic_or_and_fetch(atomic_t* ptr, atomic_t value)
{
	return __sync_or_and_fetch(ptr, value);
}

static inline atomic_t atomic_and_and_fetch(atomic_t* ptr, atomic_t value)
{
        return __sync_and_and_fetch(ptr, value);
}

static inline atomic_t atomic_xor_and_fetch(atomic_t* ptr, atomic_t value)
{
        return __sync_xor_and_fetch(ptr, value);
}

static inline atomic_t atomic_nand_and_fetch(atomic_t* ptr, atomic_t value)
{
        return __sync_nand_and_fetch(ptr, value);
}

#else

#include <asm/atomic.h>

static inline atomic_t atomic_fetch_and_add(atomic_t* ptr, atomic_t value)
{
        return __arch_fetch_and_add(ptr, value);
}

static inline atomic_t atomic_fetch_and_sub(atomic_t* ptr, atomic_t value)
{
        return __arch_fetch_and_sub(ptr, value);
}

static inline atomic_t atomic_fetch_and_or(atomic_t* ptr, atomic_t value)
{
        return __arch_fetch_and_or(ptr, value);
}

static inline atomic_t atomic_fetch_and_and(atomic_t* ptr, atomic_t value)
{
        return __arch_fetch_and_and(ptr, value);
}

static inline atomic_t atomic_fetch_and_xor(atomic_t* ptr, atomic_t value)
{
        return __arch_fetch_and_xor(ptr, value);
}

static inline atomic_t atomic_fetch_and_nand(atomic_t* ptr, atomic_t value)
{
        return __arch_fetch_and_nand(ptr, value);
}

static inline atomic_t atomic_add_and_fetch(atomic_t* ptr, atomic_t value)
{
        return __arch_add_and_fetch(ptr, value);
}

static inline atomic_t atomic_sub_and_fetch(atomic_t* ptr, atomic_t value)
{
        return __arch_sub_and_fetch(ptr, value);
}

static inline atomic_t atomic_or_and_fetch(atomic_t* ptr, atomic_t value)
{
        return __arch_or_and_fetch(ptr, value);
}

static inline atomic_t atomic_and_and_fetch(atomic_t* ptr, atomic_t value)
{
        return __arch_and_and_fetch(ptr, value);
}

static inline atomic_t atomic_xor_and_fetch(atomic_t* ptr, atomic_t value)
{
        return __arch_xor_and_fetch(ptr, value);
}

static inline atomic_t atomic_nand_and_fetch(atomic_t* ptr, atomic_t value)
{
        return __arch_nand_and_fetch(ptr, value);
}

#endif  /* __GNUC__ */

#endif
		
