#include "../include/def.h"
#include "../include/platform.h"

char ks_stack[CPUS*PAGESIZE]__attribute__((aligned(16)));
static void timer_init()
{

}

void start()
{
    while(1);
}