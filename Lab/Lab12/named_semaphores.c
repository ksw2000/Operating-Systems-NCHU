#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<fcntl.h>
#include<pthread.h>
#define nil NULL

int count = 0;
sem_t* mutex;

void* inc(void* args){
    int i = 0;
    sem_wait(mutex);
    for(i=0; i<25000000; i++){
        count++;
    }
    sem_post(mutex);

    pthread_exit(nil);
}

void* dec(void* args){
    int i = 0;
    sem_wait(mutex);
    for(i=0; i<25000000; i++){
        count--;
    }
    sem_post(mutex);

    pthread_exit(nil);
}

int main(){
    /*
        Named semaphore:
        This type of semaphore has a name, By calling sem_open() with
        the same name, unrelated processes can access the same semaphore
    */

    mutex = sem_open("/my_semaphore", O_CREAT, S_IRUSR | S_IWUSR, 1);
    if(mutex == SEM_FAILED){
        fprintf(stderr, "Failed to open semaphore for empty\n");
        exit(EXIT_FAILURE);
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

    sem_close(mutex);
    sem_unlink("/my_semaphore");

    printf("output is %d\n", count);
    pthread_exit(nil);
}
