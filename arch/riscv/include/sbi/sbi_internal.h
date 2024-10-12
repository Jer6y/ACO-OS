#ifndef __SBI_INTERNAL_H
#define __SBI_INTERNAL_H


typedef struct sbiret {
	long error;
	long value;
} sbiret_t;

#define PUT_PARAMETER_1(x)   		asm volatile("add a0, zero, %0"::"r"(x):"memory","a0");
#define PUT_PARAMETER_2(x,y)		PUT_PARAMETER_1(x)\
					asm volatile("add a1, zero, %0"::"r"(y):"memory","a0","a1");
#define PUT_PARAMETER_3(x,y,z)  	PUT_PARAMETER_2(x,y)\
					asm volatile("add a2, zero, %0"::"r"(z):"memory","a0","a1","a2");
#define PUT_PARAMETER_4(x,y,z,u)	PUT_PARAMETER_3(x,y,z)\
					asm volatile("add a3, zero, %0"::"r"(u):"memory","a0","a1","a2","a3");
#define PUT_PARAMETER_5(x,y,z,u,v)      PUT_PARAMETER_4(x,y,z,u)\
					asm volatile("add a4, zero, %0"::"r"(v):"memory","a0","a1","a2","a3","a4");
#define PUT_PARAMETER_6(x,y,z,u,v,w)    PUT_PARAMETER_5(x,y,z,u,v)\
                                        asm volatile("add a5, zero, %0"::"r"(w):"memory","a0","a1","a2","a3","a4","a5");

#define SBI_CALL(EID,FID) do {								  \
				asm volatile("add a7, zero, %0"::"r"(EID):"memory","a7"); \
				asm volatile("add a6, zero, %0"::"r"(FID):"memory","a6"); \
       				asm volatile("ecall");					  \
			     } while(0)
			
#define GET_SBIRET(x) 	 asm volatile("add %0, zero, a0":"=r"(x.error)::"memory","a0");	\
	 	 	 asm volatile("add %0, zero, a1":"=r"(x.value)::"memory","a1");	\

#endif
