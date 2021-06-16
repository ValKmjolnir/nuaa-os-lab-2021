#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct
{
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} sema_t;
void sema_init(sema_t* sema,int value)
{
    sema->value=value;
    pthread_mutex_init(&sema->mutex,NULL);
    pthread_cond_init(&sema->cond,NULL);
    return;
}
void sema_wait(sema_t* sema)
{
    pthread_mutex_lock(&sema->mutex);
    while(sema->value<=0)
        pthread_cond_wait(&sema->cond,&sema->mutex);
    sema->value--;
    pthread_mutex_unlock(&sema->mutex);
}
void sema_signal(sema_t* sema)
{
    pthread_mutex_lock(&sema->mutex);
    sema->value++;
    pthread_cond_signal(&sema->cond);
    pthread_mutex_unlock(&sema->mutex);
}

#define capacity 4
int in1=0,out1=0;
char buffer1[capacity];
sema_t mutex1;
sema_t buff1_full;
sema_t buff1_empty;

int in2=0,out2=0;
char buffer2[capacity];
sema_t mutex2;
sema_t buff2_full;
sema_t buff2_empty;

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
        sema_wait(&buff1_empty);
        sema_wait(&mutex1);
        char item='a'+i;
        put_item(item,buffer1,&in1);
        sema_signal(&mutex1);
        sema_signal(&buff1_full);
    }
    return NULL;
}
void* calculator(void* arg)
{
    for(int i=0;i<8;++i)
    {
        sema_wait(&buff1_full);
        sema_wait(&mutex1);
        char item=get_item(buffer1,&out1);
        item=item-'a'+'A';
        sema_signal(&mutex1);
        sema_signal(&buff1_empty);

        sema_wait(&buff2_empty);
        sema_wait(&mutex2);
        put_item(item,buffer2,&in2);
        sema_signal(&mutex2);
        sema_signal(&buff2_full);
    }
    return NULL;
}
void* consumer(void* arg)
{
    for(int i=0;i<8;++i)
    {
        sema_wait(&buff2_full);
        sema_wait(&mutex2);
        char item=get_item(buffer2,&out2);
        printf("consume item: %c\n",item);
        sema_signal(&mutex2);
        sema_signal(&buff2_empty);
    }
    return NULL;
}
int main()
{
    sema_init(&mutex1,1);
    sema_init(&mutex2,1);
    sema_init(&buff1_full,0);
    sema_init(&buff2_full,0);
    sema_init(&buff1_empty,capacity-1);
    sema_init(&buff2_empty,capacity-1);
    
    pthread_t producer_tid,calculator_tid,consumer_tid;
    pthread_create(&producer_tid,NULL,producer,NULL);
    pthread_create(&calculator_tid,NULL,calculator,NULL);
    pthread_create(&consumer_tid,NULL,consumer,NULL);
    pthread_join(producer_tid,NULL);
    pthread_join(calculator_tid,NULL);
    pthread_join(consumer_tid,NULL);
    return 0;
}