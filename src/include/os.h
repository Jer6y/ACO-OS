#ifndef OS_H
#define OS_H

#include "os_type.h"
#include "platform.h"


#define THR 0
#define RHR 0
#define BUAD_LOW  0 
#define BUAD_HIGH 1
#define IER 1
    #define IER_RX_INTA (1<<0)
    #define IER_TX_INTA (1<<1)
#define FCR 2
    #define FCR_FIFO_ENABLE   (1<<0)
    #define FCR_RX_FIFO_CLEAR (1<<1)
    #define FCR_TX_FIFO_CLEAR (1<<2)
#define ISR 2
#define LCR 3
    #define LCR_WORD_LENGTH (3<<0)
#define MCR 4
#define LSR 5
    #define LSR_RX_READY (1<<0)
    #define LSR_TX_READY (1<<5)
#define MSR 6
#define SPR 7

#define Reg(r) (UART_START+(r))
#define write_reg(r,val,type) (*(type *)Reg((r))=(type)(val))
#define read_reg(r,type) (*(type *)Reg((r)))


#define UART_IRQ 10

//设置PLIC 中断号为ID的权限 
#define PLIC_PRIORITY(ID)    (((PLIC_START)+(uint64)((ID)*4)))
//设置某个HART的PLIC中断使能位
#define PLIC_MENABLE(hart_id) (PLIC_START+0x2000+0x100*(hart_id))
#define PLIC_SENABLE(hart_id) (PLIC_START+0x2080+0x100*(hart_id))

#define PLIC_SCLAIM(hart_id)    (PLIC_START+0x201004+(hart_id)*0x2000)
#define PLIC_SCOMPLETE(hart_id) (PLIC_START+0x201004+(hart_id)*0x2000)

#define PLIC_SPRIORITY(hart_id) (PLIC_START+0x201000+(hart_id)*0x2000)



#define va_list(base) ((uint64*)(base)+1)
#define va_end(t)     ((t)= 0)
#define va_next(t,type) ( *(type *)((va*)((t)=(va*)(t)+1)-1))

#define PAGEUP(pgad)   (((uint64)(pgad)+PAGESIZE)&(~((uint64)PAGESIZE-1)))
#define PAGEDOWN(pgad) ((uint64)(pgad)&(~((uint64)PAGESIZE-1)))
typedef unsigned long long pagetable;
typedef unsigned long long pte_t;

typedef unsigned long long va;

/**********************************************宏定义*********************************************************
*************************************************************************************************************/

/********************************************虚拟地址管理*****************************************************/

#define PTE_V                       (1ull<<0)
#define PTE_R                       (1ull<<1)
#define PTE_W                       (1ull<<2)
#define PTE_X                       (1ull<<3)
#define MAKESATP(x)                 ((((uint64)x)>>12)|(8ull<<60))
#define PX(level,v)                 (((uint64)(v)>>((int)level*9+12))&0x1ff)
#define PA2PTE(x)                   ((((uint64)(x))>>12)<<10)
#define PTE2PA(x)                   (((uint64)(x)>>10)<<12)



/*******************************************数据类型声明********************************************************
*************************************************************************************************************/

struct      proc;
struct      trapframe;
struct      context;
enum        pstate;
enum        os_error;
struct      inode;
enum        dev_type;
struct      PHRL;
struct      memory_unit;

/************************************************************************************************************
*************************************************************************************************************/



/*******************************************数据类型定义*******************************************************
 ************************************************************************************************************/


/************内核通用错误码*************/
typedef enum os_error
{
    /*成功码*/
    OS_ERROR_DONE               ,

    /*物理内存分配错误码*/
    OS_ERROR_KFREE_UNLIGN       ,
    OS_ERROR_KALOC_NOROOM       ,
    
    /*虚拟内存分配错误码*/
    OS_ERROR_VM_AL_NOTALLOWED   ,
    OS_ERROR_VM_REMAP           ,
    OS_ERROR_VM_CANT_FIND       ,
    OS_ERROR_VM_UNMAP_TOFAIL    ,
    OS_ERROR_VM_UNMAP_PTFAIL    ,

    /*进程相关错误码*/
    OS_ERROR_PROC_NOROOMFPGT    ,
    OS_ERROR_PROC_NOROOMFPTE    ,

}os_error;

/************************************/

/************进程相关*****************/

typedef enum pstate
{
    //进程状态
    PROC_RUNNING          =0,
    PROC_SLEEPING         =1,
    PROC_READY            =2,
    PROC_ZOMBIE           =3,
    PROC_UNUSED           =4,
}pstate;

typedef struct trapframe
{
    //用户态状态信息
    uint64 ra; //0
    uint64 sp; //8
    uint64 gp; //16
    uint64 tp; //24
    uint64 t0; //32
    uint64 t1; //40
    uint64 t2;
    uint64 t3;
    uint64 t4;
    uint64 t5;
    uint64 t6;
    uint64 s0;
    uint64 s1;
    uint64 s2;    
    uint64 s3;  
    uint64 s4;  
    uint64 s5;  
    uint64 s6;  
    uint64 s7;  
    uint64 s8;  
    uint64 s9;  
    uint64 s10;  
    uint64 s11;
    uint64 a0;
    uint64 a1;
    uint64 a2;
    uint64 a3;
    uint64 a4;
    uint64 a5;
    uint64 a6;
    uint64 a7;
    uint64 sepc;
    uint64 ker_tp;
} trapframe;

typedef struct context
{
    //内核态状态信息
    uint64 ra;
    uint64 sp;
    uint64 gp;
    uint64 tp;
    uint64 t0;
    uint64 t1;
    uint64 t2;
    uint64 t3;
    uint64 t4;
    uint64 t5;
    uint64 t6;
    uint64 s0;
    uint64 s1;
    uint64 s2;    
    uint64 s3;  
    uint64 s4;  
    uint64 s5;  
    uint64 s6;  
    uint64 s7;  
    uint64 s8;  
    uint64 s9;  
    uint64 s10;  
    uint64 s11;
    uint64 a0;
    uint64 a1;
    uint64 a2;
    uint64 a3;
    uint64 a4;
    uint64 a5;
    uint64 a6;
    uint64 a7;
} context;


typedef struct proc
{
    //PCB
    //用户态的状态:
    struct trapframe    usr_regst;
    pagetable*          usr_pgt;  //256
    //内核态的状态:
    struct context      ker_regst;//264
    //进程调度信息:
    //就绪双向链表:
    struct proc*        ready_pre;
    struct proc*        ready_next;
    //阻塞双向链表:
    struct proc*        block_pre;
    struct proc*        block_next;
    pstate              proc_state;
    void*               sleep_reason;
    //进程控制信息:
    uint8               in_kernel;
    uint8               pid;
    struct proc*        proc_father;
    uint16              tick_passed;
    uint64              k_stack;
    uint64              brk_end;
    //拥有的设备
    uint8               magic_number;
} proc;

/************************************/

/***********设备文件描述符*************/

typedef enum  dev_type
{
    //设备类型
    DEV_DIR     =0,
    DEV_FILE    =1,
    DEV_CHAR    =2,
    DEV_BLOCK   =3,
} dev_type;


typedef struct inode
{
    dev_type  type;

} inode;

/************************************/

/************物理内存分配器************/

typedef struct PHRL
{
    struct PHRL* next;
} PHRL;


typedef struct memory_unit
{
    #ifdef KALLOC_SPINLOCK
    struct spinlock lk;
    #endif
    uint32 page_nums;
    uint32 page_size;
    uint32 free_pages;
    struct PHRL* next;
} memory_unit;


/*****************************************************内核数据区************************************************
*************************************************************************************************************/

/**************进程相关****************/

//内核页表
extern pagetable* kernel_pgt;

/**************物理内存分配器***********/

extern struct memory_unit kmem;

extern uint8  bitmap[TOTALPAGES];

/**************进程相关****************/

//进出临界区的判断变量
extern int             int_enable      ;
extern int             depth_int       ; 
//目前RUNNING的程序
extern struct proc*    run_proc        ;
//0级就绪队列
extern struct proc*    ready_start_z   ;
extern struct proc*    ready_end_z     ;
//1级就绪队列
extern struct proc*    ready_start_o   ;
extern struct proc*    ready_end_o     ;
//2级就绪队列
extern struct proc*    ready_start_t   ;
extern struct proc*    ready_end_t     ;
//阻塞队列
extern struct proc*    block_start     ;
extern struct proc*    block_end       ;
//僵尸队列
extern struct proc*    zombie_start    ;
extern struct proc*    zombie_end      ;

/**************************************/

/**************时钟相关****************/

//开机到现在经过的时钟tick数目
extern uint64 tick_passed;

//每个tick的ms数目
extern uint64 tick_per_ms;



/************************************/

/*************串口输出缓冲区***********/

#define OUT_BUF_SIZE 128
extern char output_buffer[OUT_BUF_SIZE];
extern uint32 output_w;
extern uint32 output_r;

/************************************/

/*************串口输入缓冲区***********/

#define INPUT_BUF_SIZE 1024

extern char input_buf[INPUT_BUF_SIZE];
extern uint32 input_r;
extern uint32 input_w;
extern uint32 input_e;

/************************************/



/*****************************************************函数方法区************************************************
*************************************************************************************************************/

/*************************************************硬件板卡启动*************************************************/

/*********************************boot.s**********************************/

void _start();      //加载tp和调度器栈帧

/*********************************start.c********************************/

void start();        //初始化时钟中断 跳转到S模式

/*********************************main.c********************************/

void main();

/*********************************handler********************************/


void timer_vec();   //时钟中断处理函数 in kernel_vec.s 


void kernel_vec();  //内核态的中断处理函数 in trap.c

/*********************************uart.c********************************/


void uart_init();                //串口初始化


int uart_getchar();             //读取单个字符


void uart_putc_sync(char c);    //同步输送字符


void uart_int_handler();        //串口的中断处理

/*********************************plic.c********************************/

void plic_init();               //初始化每个外部中断的优先级

void plic_init_hart();          //每个hart初始化hart的中断使能 以及hart的阈值

uint32 plic_claim();            // 发生外部中断时 hart 从自己所属的端口取得中断ID

void plic_complete(uint32 IID); // 处理完成中断后 向端口写EOI命令

/************************************************************************/

/*************************************************************************************************************/

/*************************************************内核初始化****************************************************/

/******************************spinlock.c********************************/

void push_on();                 //关闭中断同时记录之前的使能状态

void pop_off();                 //恢复之前的中断使能状态

/******************************kalloc.c*********************************/

void* kalloc(os_error *);                       //向物理内存分配器要1页的内存

void  kfree(uint64,os_error*);                  //释放某一页的内存

void  kfree_arrange(uint64,uint64);             //释放从[start,end]的所有页的内存

void  kinit();                                  //初始化物理内存分配器

void kbit_index_add(uint64 index);  
/******************************trap.c***********************************/

void kernel_trap();         //S-mode下trap处理的地方

void trap_init();           //初始化trap

/*****************************riscv.c************************************/

void sfence();              //读riscv-csr的c描述的函数
uint64 r_int();
uint64 r_tp();
uint64 r_sip();
uint64 r_mip();
uint64 r_sie();
uint64 r_mie();
uint64 r_satp();
uint64 r_sepc();
uint64 r_mepc();
uint64 r_stval();
uint64 r_mtval();
uint64 r_stvec();
uint64 r_mtvec();
uint64 r_scause();
uint64 r_mcause();
uint64 r_sstatus();
uint64 r_mstatus();
uint64 r_mhartid();
uint64 r_mideleg();
uint64 r_medeleg();
uint64 r_sscratch();
uint64 r_mscratch();
void w_int_on();            //写riscv-csr的c描述的函数          
void w_int_off();
void w_tp(uint64 x);
void w_sip(uint64 x);
void w_mip(uint64 x);
void w_sie(uint64 x);
void w_mie(uint64 x);
void w_satp(uint64 x);
void w_sepc(uint64 x);
void w_mepc(uint64 x);
void w_stval(uint64 x);
void w_mtval(uint64 x);
void w_stvec(uint64 x);
void w_mtvec(uint64 x);
void w_scause(uint64 x);
void w_mcause(uint64 x);
void w_sstatus(uint64 x);
void w_mstatus(uint64 x);
// hartid 为只读 是不允许写的!
// void w_mhartid(uint64 x);
void w_mideleg(uint64 x);
void w_medeleg(uint64 x);
void w_sscratch(uint64 x);
void w_mscratch(uint64 x);
void w_pmpaddr0(uint64 x);
void w_pmpcfg0(uint64 x);

/*****************************vm.c***************************************/

void        kvm_init();                                                                             //内核页表初始化

void        kvm_flush_hart();                                                                       //加载内核页表

void        kvm_map(pagetable*,uint64,uint64,uint64,int);  

pagetable*  uvm_alloc(os_error* err);                                                               //申请页表

pte_t*      walk(pagetable* table,uint64 v_ad,int is_aloc,os_error* err);                           //不建议使用
            
uint64      walkadrs(pagetable* table,uint64 v_ad,os_error* err)    ;                               //查询页表table的v_ad对应的物理地址   

void        mmap(pagetable* table,uint64 v_ad,uint64 p_ad,uint64 p_end,int perm,os_error* err,int); //映射v_ad到[p_ad,p_end);

void        uvm_map(pagetable* table,uint64 v_ad,uint64 p_ad,uint64 p_end,int perm,os_error* err,int);

void        unmap(pagetable* table,uint64 v_ad,size_t pg_size,os_error* err);                       //从v_ad 开始取消映射pg_size 页

void        free_pte(pagetable* table);                                                             //释放页表的页表项

void        free_pagetable(pagetable * table);                                                      //释放整页表 但是不会释放table

/************************************************************************/


/*****************************proc.c*************************************/

struct proc* alloc_proc(struct proc* old,os_error*)   ;  //创建一个进程 PCB与OLD一致
void         free_proc(struct  proc* this,os_error*)  ;  //释放this所指的进程
void         sleep_proc(struct proc* this,os_error*)  ;  //改变this的状态为睡眠
void         wakup_proc(void* reason,os_error*)       ;  //唤醒阻塞队列里reason的进程

/************************************************************************/

//string.c
void *memset(const void *des, uint8 c,size_t n);
void *memchr(const void *str, int c, size_t n);
void *memcpy(void *des, const void *src, size_t n);
int memcmp(const void *str1, const void *str2, size_t n);
char *strrchr(const char *str, int c);
char *strchr(const char *str, int c);
char *strcat(char *dest, const char *src);
int strcmp(const char* src,const char *des);
char *strncpy(char *dest, const char *src, size_t n);
char *strcpy(char *dest, const char *src);
size_t  strlen(const char * src);
// 以下的buf或者str都不应该超过256个字符 否则引发panic
int printf(const char* str,...);
int sprintf(char *buf, const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va* args);
void panic(char *str);
void assert_fail(char* file,int line);
#define assert(x) if(!(x)) { assert_fail(__FILE__,__LINE__); }

#endif