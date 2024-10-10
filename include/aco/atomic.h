#ifndef __ACO_ATOMIC_H
#define __ACO_ATMOIC_H

#include <aco/types.h>

typedef uint8_t atomic_t;

// if we use gcc , use gcc built-in atomic operation
// else take it to specific arch to implement it
#ifdef __GNUC__

FUNC_BUILTIN atomic_t atomic_fetch_and_add(atomic_t* ptr, atomic_t value)
{
	return __sync_fetch_and_add(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_fetch_and_sub(atomic_t* ptr, atomic_t value)
{
	return __sync_fetch_and_sub(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_fetch_and_or(atomic_t* ptr, atomic_t value)
{
	return __sync_fetch_and_or(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_fetch_and_and(atomic_t* ptr, atomic_t value)
{
	return __sync_fetch_and_and(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_fetch_and_xor(atomic_t* ptr, atomic_t value)
{
	return __sync_fetch_and_xor(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_fetch_and_nand(atomic_t* ptr, atomic_t value)
{
	return __sync_fetch_and_nand(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_add_and_fetch(atomic_t* ptr, atomic_t value)
{
	return __sync_add_and_fetch(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_sub_and_fetch(atomic_t* ptr, atomic_t value)
{
	return __sync_sub_and_fetch(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_or_and_fetch(atomic_t* ptr, atomic_t value)
{
	return __sync_or_and_fetch(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_and_and_fetch(atomic_t* ptr, atomic_t value)
{
        return __sync_and_and_fetch(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_xor_and_fetch(atomic_t* ptr, atomic_t value)
{
        return __sync_xor_and_fetch(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_nand_and_fetch(atomic_t* ptr, atomic_t value)
{
        return __sync_nand_and_fetch(ptr, value);
}

#else

#include <asm/atomic.h>

FUNC_BUILTIN atomic_t atomic_fetch_and_add(atomic_t* ptr, atomic_t value)
{
        return __arch_fetch_and_add(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_fetch_and_sub(atomic_t* ptr, atomic_t value)
{
        return __arch_fetch_and_sub(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_fetch_and_or(atomic_t* ptr, atomic_t value)
{
        return __arch_fetch_and_or(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_fetch_and_and(atomic_t* ptr, atomic_t value)
{
        return __arch_fetch_and_and(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_fetch_and_xor(atomic_t* ptr, atomic_t value)
{
        return __arch_fetch_and_xor(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_fetch_and_nand(atomic_t* ptr, atomic_t value)
{
        return __arch_fetch_and_nand(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_add_and_fetch(atomic_t* ptr, atomic_t value)
{
        return __arch_add_and_fetch(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_sub_and_fetch(atomic_t* ptr, atomic_t value)
{
        return __arch_sub_and_fetch(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_or_and_fetch(atomic_t* ptr, atomic_t value)
{
        return __arch_or_and_fetch(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_and_and_fetch(atomic_t* ptr, atomic_t value)
{
        return __arch_and_and_fetch(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_xor_and_fetch(atomic_t* ptr, atomic_t value)
{
        return __arch_xor_and_fetch(ptr, value);
}

FUNC_BUILTIN atomic_t atomic_nand_and_fetch(atomic_t* ptr, atomic_t value)
{
        return __arch_nand_and_fetch(ptr, value);
}

#endif  /* __GNUC__ */

		