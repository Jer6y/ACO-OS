#ifndef __ACO_SPINLOCK_H
#define __ACO_SPINLOCK_H

#include <aco/types.h>
#include <aco/atomic.h>

//the asm file means to the arch implement of spinlock
//that spinlock need
#include <asm/spinlock.h>

typedef struct spinlock_s {
	struct __arch_lock_s  local_lock;
	atomic_t value;
} spinlock_t;

FUNC_BUILTIN void init_lock(spinlock_t* lock)
{
	lock->value = 1;
	__arch_init_local_lock(&(lock->local_lock));
}

FUNC_BUILTIN void lock(spinlock_t* lock)
{
	__arch_local_lock(&(lock->local_lock));
	while(atomic_fetch_and_and(&(lock->value),0) == 0) ;
	return;
}

FUNC_BUILTIN void unlock(spinlock_t* lock)
{
	__arch_local_unlock(&(lock->local_lock));
	atomic_fetch_and_or(&(lock->value),1);
	return;
}

#endif /* __ACO_SPINLOCK_H */
