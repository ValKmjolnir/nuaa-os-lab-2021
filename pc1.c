#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define capacity 4
int in1=0,out1=0;
char buffer1[capacity];
pthread_mutex_t mutex1;
pthread_cond_t wait_empty1;
pthread_cond_t wait_full1;

int in2=0,out2=0;
char buffer2[capacity];
pthread_mutex_t mutex2;
pthread_cond_t wait_empty2;
pthread_cond_t wait_full2;

int buff_empty(int in,int out)
{
    return in==out;
}
int buff_full(int in,int out)
{
    return (in+1)%capacity==out;
}
char get_item(char* buff,int* out)
{
    char item=buff[*out];
    *out=(*out+1)%capacity;
    return item;
}
void put_item(char item,char* buff,int* in)
{
    buff[*in]=item;
    *in=(*in+1)%capacity;
    return;
}
void* producer(void* arg)
{
    for(int i=0;i<8;++i)
    {
        pthread_mutex_lock(&mutex1);
        while(buff_full(in1,out1))
            pthread_cond_wait(&wait_empty1,&mutex1);
        char item='a'+i;
        put_item(item,buffer1,&in1);
        pthread_cond_signal(&wait_full1);
        pthread_mutex_unlock(&mutex1);
    }
    return NULL;
}
void* calculator(void* arg)
{
    for(int i=0;i<8;++i)
    {
        pthread_mutex_lock(&mutex1);
        while(buff_empty(in1,out1))
            pthread_cond_wait(&wait_full1,&mutex1);
        char item=get_item(buffer1,&out1);
        item=item-'a'+'A';
        pthread_cond_signal(&wait_empty1);
        pthread_mutex_unlock(&mutex1);

        pthread_mutex_lock(&mutex2);
        while(buff_full(in2,out2))
            pthread_cond_wait(&wait_empty2,&mutex2);
        put_item(item,buffer2,&in2);
        pthread_cond_signal(&wait_full2);
        pthread_mutex_unlock(&mutex2);
    }
    return NULL;
}
void* consumer(void* arg)
{
    for(int i=0;i<8;++i)
    {
        pthread_mutex_lock(&mutex2);
        while(buff_empty(in2,out2))
            pthread_cond_wait(&wait_full2,&mutex2);
        char item=get_item(buffer2,&out2);
        printf("consume item: %c\n",item);
        pthread_cond_signal(&wait_empty2);
        pthread_mutex_unlock(&mutex2);
    }
    return NULL;
}
int main()
{
    pthread_mutex_init(&mutex1,NULL);
    pthread_cond_init(&wait_full1,NULL);
    pthread_cond_init(&wait_empty1,NULL);
    pthread_mutex_init(&mutex2,NULL);
    pthread_cond_init(&wait_full2,NULL);
    pthread_cond_init(&wait_empty2,NULL);
    pthread_t producer_tid,calculator_tid,consumer_tid;
    pthread_create(&producer_tid,NULL,producer,NULL);
    pthread_create(&calculator_tid,NULL,calculator,NULL);
    pthread_create(&consumer_tid,NULL,consumer,NULL);
    pthread_join(producer_tid,NULL);
    pthread_join(calculator_tid,NULL);
    pthread_join(consumer_tid,NULL);
    return 0;
}