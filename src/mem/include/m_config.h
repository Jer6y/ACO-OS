#ifndef __M_CONFIG_H
#define __M_CONFIG_H

#define M_ENABLE          1   
#define M_DISABLE         0

//resource config
#define M_POOL_SIZE         204800

#define M_PAGE_SIZE         (PAGESIZE)

#define M_MAX_ORDER         5

#define M_DEFAULT_MMPOOL    M_DISABLE

#define M_ARG_CHECK_FUNC    M_ENABLE




#ifndef M_POOL_SIZE
    #error "M_POOL_SIZE NOT DEFINE"
#endif

#ifndef M_PAGE_SIZE
    #error "M_PAGE_SIZE NOT DEFINE"
#endif

#ifndef M_MAX_ORDER
    #error "M_MAX_ORDER NOT DEFINE"
#endif


#ifndef M_ARG_CHECK_FUNC
    #error "M_ARG_CHECK_FUNC NOT DEFINE"
#endif



#endif