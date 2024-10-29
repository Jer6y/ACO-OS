#ifndef __ACO_RWLOCK_H
#define __ACO_RWLOCK_H

#include <aco/spinlock.h>
#include <aco/atomic.h>
#include <aco/cpu_critical.h>

typedef struct rwlock {
	spinlock_t lk;
	atomic_t   readers;
	atomic_t   writers;
} rwlock_t;


static inline void rwlock_init(rwlock_t* ptr)
{
	atomic_init(&(ptr->writers),0);
	atomic_init(&(ptr->readers),0);
	init_lock(&(ptr->lk));
	return;
}

static inline void r_lock(rwlock_t* ptr)
{
	while(1)
	{
		cpu_critical_in();
		lock(&(ptr->lk));
		if(atomic_fetch_and_add(&(ptr->writers),0) == 0)
		{
			atomic_fetch_and_add(&(ptr->readers),1);
			unlock(&(ptr->lk));
			break;
		}
		unlock(&(ptr->lk));
		cpu_critical_out();
	}
	return;
}

static inline void r_unlock(rwlock_t* ptr)
{
	atomic_fetch_and_sub(&(ptr->readers),1);
	cpu_critical_out();
}

static inline void w_lock(rwlock_t* ptr)
{
	while(1)
	{
		cpu_critical_in();
		lock(&(ptr->lk));
		if(atomic_fetch_and_add(&(ptr->writers),0) == 0 && atomic_fetch_and_add(&(ptr->readers),0) ==0)
		{
			atomic_fetch_and_add(&(ptr->writers),1);
			unlock(&(ptr->lk));
			break;
		}
		unlock(&(ptr->lk));
		cpu_critical_out();
	}

}

static inline void w_unlock(rwlock_t* ptr)
{
	atomic_fetch_and_sub(&(ptr->writers),1);
	cpu_critical_out();
}

//writer lock and block readers
static inline void w_lock_br(rwlock_t* ptr)
{
	while(1)
	{
		cpu_critical_in();
		lock(&(ptr->lk));
		if(atomic_fetch_and_add(&(ptr->writers),0) == 0)
		{
			atomic_fetch_and_add(&(ptr->writers),1);
			unlock(&(ptr->lk));
			break;
		}
		unlock(&(ptr->lk));
		cpu_critical_out();
	}
	while(atomic_fetch_and_add(&(ptr->readers),0) != 0) ;
}

static inline void w_unlock_br(rwlock_t* ptr)
{
	atomic_fetch_and_sub(&(ptr->writers),1);
	cpu_critical_out();
}

#endif /* __ACO_RWLOCK_H */
