#include<stdio.h>
#include<semaphore.h>
#include<pthread.h>
#define nil NULL

int count = 0;
sem_t mutex;

void* inc(void* args){
    int i = 0;
    sem_wait(&mutex);
    for(i=0; i<25000000; i++){
        count++;
    }
    sem_post(&mutex);

    pthread_exit(nil);
}

void* dec(void* args){
    int i = 0;
    sem_wait(&mutex);
    for(i=0; i<25000000; i++){
        count--;
    }
    sem_post(&mutex);

    pthread_exit(nil);
}

int main(){
    /*
        int sem_init(sem_t *sem, int pshared, unsigned value);
        return -1 if unsuccessful

        pshared: a flag indicaiting whether or not the semaphore should be shared with forked processes.

        pshared = 0 only threads of process creating
        semaphore can use semaphore

        value: 可以同時有幾個執行緒使用該鎖 (value > 0)
    */
    if(sem_init(&mutex, 0, 1) == -1){
        fprintf(stderr, "sem_init() error\n");
    }

    int i=0;
    pthread_t id[4];
    pthread_create(&id[0], nil, dec, nil);
    pthread_create(&id[1], nil, inc, nil);
    pthread_create(&id[2], nil, dec, nil);
    pthread_create(&id[3], nil, inc, nil);

    for(i=0; i<4; i++){
        pthread_join(id[i], nil);
    }

    printf("output is %d\n", count);
    pthread_exit(nil);
}
