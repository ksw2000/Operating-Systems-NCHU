#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#define nil NULL

int* result;
void* child(void *arg){
    int* input = (int*) arg;
    result = malloc(sizeof(int));
    result[0] = input[0] + input[1];

    printf("Sleep:1\n");
    sleep(1);
    printf("Sleep:2\n");
    sleep(1);
    printf("Sleep:3\n");
    sleep(1);

    pthread_exit((void*)result);
}

int main(){
    pthread_t t;
    int input[] = {1, 2};   // input data

    // create a child thread, pass input to calculate
    pthread_create(&t, nil, child, (void*)input);

    // wait child thread
    pthread_join(t, nil);
    int p_cancel = pthread_cancel(t);
    if(!p_cancel){
        printf("pthread_cancel successfully\n");
    }

    // output the result of calculation
    printf("%d + %d = ", *(input+0), *(input+1));
    printf("%d\n", *result);

    free(result);

    return 0;
}
