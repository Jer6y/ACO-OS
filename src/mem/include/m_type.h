#ifndef __M_TYPE_H
#define __M_TYPE_H

typedef enum m_type_s {
    OBJ_M_BLK = 2763u,
    OBJ_M_POOL       ,
    OBJ_M_SLAB       ,
} m_type_t;

// this tag for a function
#define M_API               //means function can be used in other modules
#define M_STATIC static     //means function is private


#endif