#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<mqueue.h>
#define MY_MQ "/q2uque"
#define MAX_SIZE 1000

int main(int argc, char** argv){
    mqd_t mql;
    char buffer[MAX_SIZE];
    mql = mq_open(MY_MQ, O_WRONLY | O_CREAT, 0644, NULL);
    if(mql == -1){
        perror("mql error:\n");
    }
    printf("send to receiver:\n");
    int i, send;
    while(1){
        for(i=0; i<sizeof(buffer); i++){
            buffer[i] = '\0';
        }
        printf(">");
        fgets(buffer, MAX_SIZE, stdin);
        send = mq_send(mql, buffer, strlen(buffer)+1, 0);
        if(send < 0){
            perror("message:\n");
            exit(1);
        }
        if(!strncmp(buffer, "exit\n", strlen(buffer))){
            return 0;
            if(mq_unlink(MY_MQ) == -1){
                exit(0);
            }
        }
    };
    return 0;
}
