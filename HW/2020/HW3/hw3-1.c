#include<stdio.h>
#include<pthread.h>

int counter = 0;
int buffer[5] = {0};

void* producer(void* arg){
    for(;;){
        if(counter < 5){
            buffer[counter++] = 1;
            printf("run producer count=%d\n", counter);
        }
    }
}

void* consumer(void* arg){
    for(;;){
        if(counter > 0){
            buffer[--counter] = 0;
            printf("run consumer count=%d\n", counter);
        }
    }
}

int main(){
    pthread_t p1, p2, p3, p4;
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
