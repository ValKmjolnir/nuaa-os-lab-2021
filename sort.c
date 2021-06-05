#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
struct param
{
	int* arr;
	unsigned int begin;
	unsigned int end;
};
void sort(int* arr,int left,int right)
{
	if(left>=right)return;
	int l=left,r=right;
	int tmp=arr[l];
	while(left<right)
	{
		while(left<right && tmp<=arr[right])--right;
		while(left<right && tmp>=arr[left])++left;
		if(left!=right)
		{
			int t=arr[left];
			arr[left]=arr[right];
			arr[right]=t;
		}
	}
	arr[l]=arr[left];
	arr[left]=tmp;
	sort(arr,l,left-1);
	sort(arr,left+1,r);
	return;
}
void* calc(void* arg)
{
	struct param* par=(struct param*)arg;
	sort(par->arr,par->begin,par->end);
	return NULL;
}
void merge(int* arr)
{
	int tmp[4096];
	for(int i=0;i<4096;++i)
		tmp[i]=arr[i];
	int i=0,j=2048,index=0;
	while(1)
	{
		if(i==2048||j==4096)
			break;
		if(tmp[i]<=tmp[j])
		{
			arr[index]=tmp[i];
			++index;
			++i;
		}
		else
		{
			arr[index]=tmp[j];
			++index;
			++j;
		}
	}
	for(;j<4096;++j,++index)
		arr[index]=tmp[j];
	for(;i<2048;++i,++index)
		arr[index]=tmp[i];
	return;
}
int main()
{
	pthread_t worker_tid[2];
	struct param pars[2];
	int arr[4096];
	srand((unsigned)time(NULL));
	for(int i=0;i<4096;++i)
		arr[i]=rand()%1000;
	pars[0].arr=arr;
	pars[0].begin=0;
	pars[0].end=2047;
	pars[1].arr=arr;
	pars[1].begin=2048;
	pars[1].end=4095;
	pthread_create(&worker_tid[0],NULL,calc,&pars[0]);
	pthread_create(&worker_tid[1],NULL,calc,&pars[1]);
	pthread_join(worker_tid[0],NULL);
	pthread_join(worker_tid[1],NULL);
	
	merge(arr);
	for(int i=0;i<4096;++i)
		printf("%d ",arr[i]);
	printf("\n");
	return 0;
}
