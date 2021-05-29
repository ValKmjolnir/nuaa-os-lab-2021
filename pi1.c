#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

double worker_sum,master_sum;

void* worker(void* arg)
{
	worker_sum=0;
	int opr=0;
	for(unsigned int i=0;i<524288;++i)
	{
		if(opr)
			worker_sum-=1.0/(1.0+i*2.0);
		else
			worker_sum+=1.0/(1.0+i*2.0);
		opr=~opr;
	}
	return NULL;
}
void* master(void* arg)
{
	master_sum=0;
	int opr=0;
	for(unsigned int i=524288;i<1048576;++i)
	{
		if(opr)
			master_sum-=1.0/(1.0+i*2.0);
		else
			master_sum+=1.0/(1.0+i*2.0);
		opr=~opr;
	}
	return NULL;
}
int main()
{
	pthread_t worker_tid;
	double total;
	pthread_create(&worker_tid,NULL,worker,NULL);
	master(NULL);
	pthread_join(worker_tid,NULL);
	total=master_sum+worker_sum;
	printf("%lf\n",total*4);
	return 0;
}
