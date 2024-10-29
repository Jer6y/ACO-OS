#ifndef __ACO_CPU_CRITICAL_H
#define __ACO_CPU_CRITICAL_H

/*
 *  for the implemetation , we must nested support!!
 *
 *  .eg.
 *  {
 *  	cpu_critical_in();
 *  	cpu_critical_in();
 *  	cpu_critical_in();
 *  	cpu_critical_out();
 *  	cpu_critical_out();
 *  	cpu_critical_out();
 *  }
 *  and such situation is safe
 *  .eg.
 *  {
 *  	cpu_critical_in();
 *  	cpu_critical_in();
 *  	cpu_critical_out();
 *  	cpu_critical_out();
 *  	cpu_critical_out();
 *  }
 */

/*  Desc : let cpu get into the CPU_CRITICAL_ZONE
 *  Param: 
 *  Ret  : 
 *  		int : return the nest-depth for the CRITICAL_ZONE after jump in
 *  Usage:
 *  #include <aco/cpu_critical.h>
 *  void test(void)
 *  {
 *  	int x = cpu_critical_in();
 *	// x -> 1
 *	x = cpu_critical_in();
 *	// x -> 2
 *	x = cpu_critical_in();
 *	// x -> 3
 *	x = cpu_critical_out();
 *  }
 */
int cpu_critical_in();

/*  Desc : let cpu get out of the CPU_CRITICAL_ZONE
 *  Param:
 *  Ret  :
 *              int : return the nest-depth for the CRITICAL_ZONE after jump out
 *  Usage:
 *  #include <aco/cpu_critical.h>
 *  void test(void)
 *  {
 *      int x = cpu_critical_in();
 *      // x -> 1
 *      x = cpu_critical_in();
 *      // x -> 2
 *      x = cpu_critical_in();
 *      // x -> 3
 *      x = cpu_critical_out();
 *      // x -> 2
 *      x = cpu_critical_out();
 *	// x -> 1
 *	x = cpu_critical_out();
 *	// x -> 0
 *	x = cpu_critical_out();
 *	// x -> 0
 *  }
 */
int cpu_critical_out();

#endif /* __ACO_CPU_CRITICAL_H */
