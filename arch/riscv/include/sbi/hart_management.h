#ifndef __ACO_HART_MANAGMENT_H
#define __ACO_HART_MANAGMENT_H

#include "sbi_internal.h"

#define  HSM_EID       0x48534d
#define  FID_START_HART	      	0
#define  FID_STOP_HART	 	1
#define  FID_GET_HART_STATUS    2
#define  FID_SUSPEND_HART	3

static inline struct sbiret sbi_hart_start(unsigned long hartid,\
					   unsigned long start_addr,\
					   unsigned long opaque)
{
        struct sbiret ret;
        PUT_PARAMETER_3(hartid,start_addr,opaque);
        SBI_CALL(HSM_EID,FID_START_HART);
        GET_SBIRET(ret);
        return ret;
}

static inline struct sbiret sbi_hart_stop(void)
{
	struct sbiret ret;
	SBI_CALL(HSM_EID,FID_STOP_HART);
	GET_SBIRET(ret);
	return ret;
}

static inline struct sbiret sbi_hart_get_status(unsigned long hartid)
{
	struct sbiret ret;
	PUT_PARAMETER_1(hartid);
	SBI_CALL(HSM_EID,FID_GET_HART_STATUS);
	GET_SBIRET(ret);
	return ret;
}

static inline struct sbiret sbi_hart_suspend(unsigned int suspend_type,\
					     unsigned long resume_addr,\
					     unsigned long opaque)
{
	struct sbiret ret;
	PUT_PARAMETER_3(suspend_type, resume_addr, opaque);
	SBI_CALL(HSM_EID,FID_SUSPEND_HART);
	GET_SBIRET(ret);
	return ret;
}



#endif /* __ACO_HART_MANAGMENT_H */
