#include <aco/types.h>
#include <mm/slab.h>
#include <aco/init.h>
#include <aco/log.h>

kmem_cache_t slab_kmm_cache;
DEFINE_ATOMIC(kmem_module_init_done,0);

FUNC_BUILTIN int  kmem_module_init(void)
{
	if(atomic_fetch_and_add(&kmem_module_init_done,0) != 0)
                return 0;
	int ret;
        ret = kmem_cache_init(&slab_kmm_cache, sizeof(slab_kmm_cache), 0, 3, 3);
        if(ret < 0 )
        {
                log_fatal("[KMEM_CACHE]: slab for kmm cache init error!");
                return ret;
        }
        log_debug("[KMEM_CACHE]: slab kmem_cache objnum : %d",slab_kmm_cache.obj_num);
        log_debug("[KMEM_CACHE]: slab kmem_cache objsiz : %d",slab_kmm_cache.obj_size);
        log_debug("[KMEM_CACHE]: slab kmem_cache pgordr : %d",slab_kmm_cache.pg_order);
        log_debug("[KMEM_CACHE]: slab kmem_cache pglimt : %d",slab_kmm_cache.pg_orlimit);
        log_debug("[KMEM_CACHE]: slab for kmm cache init done!");
	atomic_fetch_and_add(&kmem_module_init_done,1);
        return ret;
}

FUNC_BUILTIN void kmem_module_exit(void)
{
        return;
}

REGISTER_MODULE_INIT(PRIO_3, kmem_module_init);
REGISTER_MODULE_EXIT(PRIO_3, kmem_module_exit);

