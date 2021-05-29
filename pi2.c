#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#define NR_CPU 4
struct param
{
	unsigned int begin;
	unsigned int end;
};
struct result
{
	double sum;
};

void* calc(void* arg)
{
	struct param* par=(struct param*)arg;
	struct result* res=(struct result*)malloc(sizeof(struct result));
	double sum=0;
	int opr=0;
	for(unsigned int i=par->begin;i<par->end;++i)
	{
		if(opr)
			sum-=1.0/(1.0+i*2.0);
		else
			sum+=1.0/(1.0+i*2.0);
		opr=~opr;
	}
	res->sum=sum;
	return (void*)res;
}

int main()
{
	pthread_t worker_tid[NR_CPU];
	struct param pars[NR_CPU];
	double sum=0;
	for(unsigned int i=0;i<NR_CPU;++i)
	{
		pars[i].begin=i*1048576;
		pars[i].end=(i+1)*1048576;
		pthread_create(&worker_tid[i],NULL,calc,&pars[i]);
	}
	for(int i=0;i<NR_CPU;++i)
	{
		struct result* res;
		pthread_join(worker_tid[i],(void**)&res);
		sum+=res->sum;
		free(res);
	}
	printf("%lf\n",sum*4);
	return 0;
}
