/*
    compile:
    gcc Exercise2.c -pthread -o Exercise2
    run
    ./Exercise2
*/
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

pthread_mutex_t mutex;
pthread_cond_t c1, c2, c3;

void* one(void* args){
    pthread_mutex_lock(&mutex);
    for(;;){
        pthread_cond_wait(&c1, &mutex);
        printf("1 ");
        pthread_cond_signal(&c2);
    }
    pthread_mutex_unlock(&mutex);
}

void* three(void* args){
    pthread_mutex_lock(&mutex);
    for(;;){
        pthread_cond_wait(&c2, &mutex);
        printf("3 ");
        pthread_cond_signal(&c3);
    }
    pthread_mutex_unlock(&mutex);
}

void* five(void* args){
    pthread_mutex_lock(&mutex);
    for(;;){
        pthread_cond_wait(&c3, &mutex);
        printf("5 ");
        pthread_cond_signal(&c1);
    }
    pthread_mutex_unlock(&mutex);
}

int main(){
    if(pthread_mutex_init(&mutex, NULL)){
        fprintf(stderr, "pthread_mutex_init() error\n");
    }

    pthread_cond_init(&c1, NULL);
    pthread_cond_init(&c2, NULL);
    pthread_cond_init(&c3, NULL);

    pthread_t t1, t2, t3;
    pthread_create(&t1, NULL, one, NULL);
    pthread_create(&t2, NULL, three, NULL);
    pthread_create(&t3, NULL, five, NULL);

    pthread_cond_signal(&c1);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&c1);
    pthread_cond_destroy(&c2);
    pthread_cond_destroy(&c3);

    return 0;
}
