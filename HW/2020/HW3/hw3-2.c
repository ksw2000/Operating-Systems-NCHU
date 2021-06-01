#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>

int count = 0;
int buffer[5] = {0};
sem_t mutex;

void* producer(void* arg){
    for(;;){
        sem_wait(&mutex);
        if(count < 5){
            buffer[count++] = 1;
            printf("run producer count=%d\n", count);
        }
        sem_post(&mutex);
    }
    pthread_exit(NULL);
}

void* consumer(void* arg){
    for(;;){
        sem_wait(&mutex);
        if(count > 0){
            buffer[--count] = 0;
            printf("run consumer count=%d\n", count);
        }
        sem_post(&mutex);
    }
    pthread_exit(NULL);
}

int main(){
    pthread_t p1, p2, p3, p4;

    if(sem_init(&mutex, 0, 1) == -1){
        fprintf(stderr, "Failed to initialize semaphore\n");
        exit(EXIT_FAILURE);
    }

    pthread_create(&p1, NULL, producer, NULL);
    pthread_create(&p2, NULL, producer, NULL);
    pthread_create(&p3, NULL, consumer, NULL);
    pthread_create(&p4, NULL, consumer, NULL);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);
    pthread_join(p4, NULL);
    return 0;
}
