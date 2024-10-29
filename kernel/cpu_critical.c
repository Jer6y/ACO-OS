#include <aco/cpu.h>
#include <aco/cpu_critical.h>

//let it into bss , 0 is the right inital value
//to do a such trick, we no need init function to be called to 
//let INTR_NEST[0...CPUS-1] = 0;
//let INTR_ENABLE[0...CPUS-1] = 0;
//any more
int INTR_NEST[CPUS];
int INTR_ENABLE[CPUS];

int cpu_critical_in()
{
        int local_cpu_state = cpu_intr_get();
        cpu_intr_off();
        int cpuid = cpu_getid();
        if(INTR_NEST[cpuid] == 0)
                INTR_ENABLE[cpuid] = local_cpu_state;
        INTR_NEST[cpuid]++;
        return INTR_NEST[cpuid];
}

int cpu_critical_out()
{
        int local_cpu_state = cpu_intr_get();
        cpu_intr_off();
        int cpuid = cpu_getid();
        if(INTR_NEST[cpuid] ==0)
        {
                cpu_intr_restore(local_cpu_state);
                return 0;
        }
        INTR_NEST[cpuid]--;
        if(INTR_NEST[cpuid] == 0)
        {
                cpu_intr_restore(INTR_ENABLE[cpuid]);
                return 0;
        }
        return INTR_NEST[cpuid];
}
