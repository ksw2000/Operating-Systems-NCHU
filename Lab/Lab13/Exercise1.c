/*
    compile:
    gcc Exercise1.c -pthread -o Exercise1
    run:
    ./Exercise1
*/
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

int count = 0;
pthread_mutex_t mutex;

void* add(void* args){
    int i;
    pthread_mutex_lock(&mutex);
    for(i=0; i<1000000; i++){
        count++;
    }
    pthread_mutex_unlock(&mutex);
}

void* sub(void *args){
    int i;
    pthread_mutex_lock(&mutex);
    for(i=0; i<1000000; i++){
        count--;
    }
    pthread_mutex_unlock(&mutex);
}

int main(){
    pthread_t t1, t2, t3;

    // return 0 if success
    if(pthread_mutex_init(&mutex, NULL)){
        fprintf(stderr, "pthread_mutex_init() error\n");
    }

    pthread_create(&t1, NULL, add, NULL);
    pthread_create(&t2, NULL, add, NULL);
    pthread_create(&t3, NULL, sub, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    printf("count:%d\n", count);

    pthread_mutex_destroy(&mutex);
    return 0;
}
