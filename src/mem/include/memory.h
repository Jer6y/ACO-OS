#ifndef __MEMORY_H
#define __MEMORY_H

#include "m_type.h"
#include "m_config.h"
#include "m_port_user.h"
#include "mln_list.h"


#include "m_blk.h"
#include "m_pool.h"
#include "m_slab.h"


#if (M_DEFAULT_MMPOOL == M_ENABLE)
extern char default_memory_pool[M_POOL_SIZE];
extern m_pool_t d_mmpool_handler;
#endif

void module_memory_init();



#endif