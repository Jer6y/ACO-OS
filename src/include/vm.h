#ifndef VM_H
#define VM_H
    #define PTE_V  (1ull<<0)
    #define PTE_R  (1ull<<1)
    #define PTE_W  (1ull<<2)
    #define PTE_X  (1ull<<3)
    #define MAKESATP(x) ((((uint64)x)>>12)|(8ull<<60))
    #define PX(level,v) (((uint64)(v)>>((int)level*9+12))&0x1ff)
    #define PA2PTE(x) ((((uint64)(x))>>12)<<10)
    #define PTE2PA(x) (((uint64)(x)>>10)<<12)

#endif