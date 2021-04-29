#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#define nil NULL

static __thread int glob = 0;
static void incr(int loops){
    int loc, j;
    for(j=0; j<loops; j++){
        loc = glob+1;
        glob = loc;
    }
}

void* function(void *arg){
    glob++;
    incr(*(int*)arg);
    printf("thread id = %lu, glob = %d\n", pthread_self(), glob);
    pthread_exit(nil);
}

int main(){
    pthread_t id[2];
    int fifty = 49;
    int hundred = 99;
    pthread_create(&id[0], nil, function, &fifty);
    pthread_create(&id[1], nil, function, &hundred);

    pthread_join(id[0], nil);
    pthread_join(id[1], nil);

    return 0;
}
