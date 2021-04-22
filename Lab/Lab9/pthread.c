#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
int count = 0;

void* increase(void *arg){
    printf("Thread, ID is: %lu completed.\n", pthread_self());
    int i;
    for(i=0; i<250000;i++){
        count++;
    }

    pthread_exit(0);
}

int main(){
    pthread_t thread;
    int rc;
    rc = pthread_create(&thread, NULL, increase, NULL);
    if(rc){
        fprintf(stderr, "CREATE_ERROR");
        exit(-1);
    }

    rc = pthread_create(&thread, NULL, increase, NULL);
    if(rc){
        fprintf(stderr, "CREATE_ERROR");
        exit(-1);
    }

    rc = pthread_create(&thread, NULL, increase, NULL);
    if(rc){
        fprintf(stderr, "CREATE_ERROR");
        exit(-1);
    }

    rc = pthread_create(&thread, NULL, increase, NULL);
    if(rc){
        fprintf(stderr, "CREATE_ERROR");
        exit(-1);
    }

    rc = pthread_join(thread, NULL);
    if(rc){
        fprintf(stderr, "JOIN ERROR");
        exit(-1);
    }
    printf("the count value is: %d\n", count);
    return 0;
}
