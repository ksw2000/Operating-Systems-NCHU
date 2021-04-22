#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<mqueue.h>
#include<pthread.h>
#define MY_MQ1 "/q2uque2"
#define MY_MQ2 "/q2uque1"
#define MAX_SIZE 1000
char buffer1[MAX_SIZE+1];
char buffer2[MAX_SIZE+1];
int stop = 0;
void* mysender(){
    mqd_t mql = mq_open(MY_MQ1, O_WRONLY | O_CREAT, 0644, NULL);
    if(mql == -1){
        perror("mql error:\n");
    }

    int i, send;
    for(i=0; i<sizeof(buffer1); i++){
        buffer1[i] = '\0';
    }

    while(!stop){
        printf("> ");
        fgets(buffer1, MAX_SIZE, stdin);

        send = mq_send(mql, buffer1, strlen(buffer1)+1, 0);
        if(send < 0){
            perror("message:\n");
            exit(1);
        }
    }

    if(mq_close(mql) == -1){
        perror("s_close error:\n");
        exit(1);
    }
    if(mq_unlink(MY_MQ1) == -1){
        exit(0);
    }
    pthread_exit(0);
}

void* myreiver(){
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = MAX_SIZE,
        .mq_curmsgs = 0
    };
    mqd_t mql = mq_open(MY_MQ2, O_RDONLY | O_CREAT, 0644, &attr);
    if(mql == -1){
        perror("mq error:\n");
        exit(1);
    }

    size_t bytes_read;
    while(!stop){
        // default max msgsize is 8192 byte.
        // 8192 = 2^13
        bytes_read = mq_receive(mql, buffer2, 8192, NULL);

        if(bytes_read < 0){
            perror("bytMY_MQes_read error\n");
            exit(1);
        }
        buffer2[bytes_read] = '\0';
        if(!strncmp(buffer2, "exit", strlen("exit"))){
            stop = 1;
        }else{
            printf("received: %s", buffer2);
        }

        if(!strcmp(buffer2, "exit\n")){
            stop = 1;
        }
    }

    if(mq_close(mql) == -1){
        perror("s_close error:\n");
        exit(1);
    }

    if(mq_unlink(MY_MQ2) == -1){
        exit(0);
    }

    pthread_exit(0);
}

int main(int argc, char** argv){
    pthread_t thread1;
    pthread_t thread2;
    int rc;
    rc = pthread_create(&thread1, NULL, mysender, NULL);
    if(rc){
        fprintf(stderr, "CREATE_ERROR");
        exit(-1);
    }

    rc = pthread_create(&thread2, NULL, myreiver, NULL);
    if(rc){
        fprintf(stderr, "CREATE_ERROR");
        exit(-1);
    }

    rc = pthread_join(thread1, NULL);
    if(rc){
        fprintf(stderr, "JOIN ERROR");
        exit(-1);
    }
    rc = pthread_join(thread2, NULL);
    if(rc){
        fprintf(stderr, "JOIN ERROR");
        exit(-1);
    }

    return 0;
}
