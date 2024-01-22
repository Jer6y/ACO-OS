#include "include/memory.h"

#if (M_DEFAULT_MMPOOL == M_ENABLE)
    char default_memory_pool[M_POOL_SIZE];
    m_pool_t d_mmpool_handler;
#endif

void module_memory_init()
{
#if (M_DEFAULT_MMPOOL == M_ENABLE)
    m_pool_init(&d_mmpool_handler,default_memory_pool,M_POOL_SIZE);
#endif
}