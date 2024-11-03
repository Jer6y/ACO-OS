#include <aco/log.h>
#include <aco/init.h>
#include <aco/errno.h>
#include <aco/types.h>
#include <aco/spinlock.h>

DEFINE_ATOMIC(log_init_done,0);
static spinlock_t log_lock;

FUNC_BUILTIN void lock_function(bool is_lock, void *udata)
{
	spinlock_t* lk = (spinlock_t*)udata;
	if(is_lock)
	{
		lock(lk);
	}
	else
	{
		unlock(lk);
	}
	return;
}

FUNC_BUILTIN int log_module_init(void)
{
	// just init once is ok
	if(atomic_fetch_and_add(&log_init_done, 0) != 0)
		return 0;
	int ret;
	init_lock(&log_lock);
#if (CONFIG_LOG_LEVEL_TRACE == 1)
	log_set_level(LOG_TRACE);
#elif (CONFIG_LOG_LEVEL_DEBUG == 1)
	log_set_level(LOG_DEBUG);
#elif (CONFIG_LOG_LEVEL_INFO == 1)
	log_set_level(LOG_INFO);
#elif (CONFIG_LOG_LEVEL_WARN == 1)
	log_set_level(LOG_WRAN);
#elif (CONFIG_LOG_LEVEL_ERROR == 1)
	log_set_level(LOG_ERROR);
#elif (CONFIG_LOG_LEVEL_FATAL == 1)
	log_set_level(LOG_FATAL);
#else
	return -EINVAL;	
#endif
	log_set_lock(lock_function, &log_lock);
	log_set_quiet(0);
	atomic_fetch_and_add(&log_init_done, 1);
        return 0;
}

FUNC_BUILTIN void log_module_exit(void)
{
	return;
}

REGISTER_MODULE_INIT(PRIO_0, log_module_init);
REGISTER_MODULE_EXIT(PRIO_0, log_module_exit);
