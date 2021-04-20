#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<mqueue.h>
#define MY_MQ "/q2uque"
#define MSG_STOP "/0"
#define MAX_SIZE 1000

int main(int argc, char** argv){
    char buffer[MAX_SIZE+1];
    int must_stop = 0;
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = MAX_SIZE,
        .mq_curmsgs = 0
    };
    mqd_t mql = mq_open(MY_MQ, O_RDONLY | O_CREAT, 0644, &attr);
    if(mql == -1){
        perror("mq error:\n");
        exit(1);
    }
    size_t bytes_read;
    while(!must_stop){
        // default max msgsize is 8192 byte.
        // 8192 = 2^13
        bytes_read = mq_receive(mql, buffer, 8192, NULL);
        if(bytes_read < 0){
            perror("bytMY_MQes_read error\n");
            exit(1);
        }
        buffer[bytes_read] = '\0';
        if(!strncmp(buffer, MSG_STOP, strlen(MSG_STOP))){
            must_stop = 1;
        }else{
            printf("received: %s", buffer);
        }

        if(!strcmp(buffer, "exit\n")){
            must_stop = 1;
        }
    }
    if(mq_close(mql) == -1){
        perror("s_close error:\n");
        exit(1);
    }
    if(mq_unlink(MY_MQ) == -1){
        exit(0);
    }
    return 0;
}
