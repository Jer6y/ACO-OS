#ifndef __ACO_SPINLOCK_H
#define __ACO_SPINLOCK_H

#include <aco/types.h>
#include <aco/atomic.h>

#include <aco/cpu_critical.h>
typedef struct spinlock_s {
	atomic_t value;
} spinlock_t;

FUNC_BUILTIN void init_lock(spinlock_t* lock)
{
	lock->value = 1;
}

FUNC_BUILTIN void lock(spinlock_t* lock)
{
	cpu_critical_in();
	while(atomic_fetch_and_and(&(lock->value),0) == 0) ;
	return;
}

FUNC_BUILTIN void unlock(spinlock_t* lock)
{
	atomic_fetch_and_or(&(lock->value),1);
	cpu_critical_out();
	return;
}
/*
 * Support nest lock and disorder unlcok
 * and only have one lock locked , cpu are still in CRITICAL TIME
 * only all lock unlock , cpu are exit CPU_CRITICAL TIME
 * .eg. 
 * {
 * 	lock(A);
 * 	lock(B);
 * 	lock(C);
 * 	unlock(C);
 * 	unlock(B);
 * 	unlock(A);
 * }
 *
 * .e.g
 * {
 * 	lock(A);
 * 	lock(B);
 * 	lock(C);
 * 	unlock(B);
 * 	unlock(C);
 * 	unlock(A);
 * }
 *
 */

#define SPINLOCK_INIT_VAL	{ ATOMIC_INIT_VAL(1) }

#define DEFINE_SPINLOCK(name) 	static spinlock_t name = SPINLOCK_INIT_VAL


#endif /* __ACO_SPINLOCK_H */
