#include <aco/log.h>
#include <aco/init.h>
#include <aco/errno.h>
#include <aco/types.h>

FUNC_BUILTIN void lock_function(bool lock, void *udata)
{
	if(lock)
	{
	}
	else
	{
	}
	return;
}

int log_module_init(void)
{
	int ret;
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
	log_set_lock(lock_function, NULL);
	log_set_quiet(0);
        return 0;
}

void log_module_exit(void)
{
        return;
}

REGISTER_MODULE_INIT(PRIO_2, log_module_init);
REGISTER_MODULE_EXIT(PRIO_2, log_module_exit);
