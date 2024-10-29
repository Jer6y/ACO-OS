#include <aco/atomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREAD_NUM 10000

static atomic_t  val = 0;
static atomic_t  min = 200000;
static pthread_t tid[THREAD_NUM];

void* do_excute_thread(void* arg)
{
	for(int i=0; i<10; i++)
		atomic_fetch_and_add(&val,1);
	for(int i=0; i<10; i++)
		atomic_add_and_fetch(&val,1);
	for(int i=0; i<10; i++)
		atomic_fetch_and_sub(&min,1);
	for(int i=0; i<10; i++)
		atomic_sub_and_fetch(&min,1);
	return NULL;
}	
/*
void* do_excute_thread(void* arg)
{
	for(int i=0; i<10; i++)
		val++;
	for(int i=0; i<10; i++)
		val++;
	for(int i=0; i<10; i++)
		min--;
	for(int i=0; i<10; i++)
		min--;
	return NULL;
}
*/
int main(int argc,char* argv[])
{
	printf("add prev : %d\n", val);
	printf("sub prev : %d\n", min);
	for(int i=0; i<THREAD_NUM; i++)
	{
		pthread_create(&(tid[i]), NULL, do_excute_thread, NULL);
	}
	for(int i=0; i<THREAD_NUM; i++)
        {
                pthread_join(tid[i], NULL);
        }
	printf("add afte : %d\n", val);
	printf("sub afte : %d\n", min);
	return 0;
}

