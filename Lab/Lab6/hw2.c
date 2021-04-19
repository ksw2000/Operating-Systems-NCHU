#include<stdio.h>
#include<sys/wait.h>
#include<unistd.h>


int main(){
    pid_t new_pid;
    pid_t new_pid2;
    pid_t new_pid3;
    pid_t new_pid4;
    new_pid = fork();
    switch(new_pid){
        case -1:
            printf("fork error\n");
            break;
        case 0:
            //printf("\nI'm child process B.\n");
            //printf("Pid:%d\nParent pid:%d\n", getpid(), getppid());
            //printf("---------------------------------------------\n");
            new_pid4 = fork();
            switch(new_pid4){
                case -1:
                    printf("fork error\n");
                    break;
                case 0:
                    printf("\nI'm child process C.\n");
                    printf("Pid:%d\nParent pid:%d\n", getpid(), getppid());
                    printf("---------------------------------------------\n");
                    break;
                default:
                    wait(NULL);
                    printf("\nI'm child process B.\n");
                    printf("Pid:%d\nParent pid:%d\n", getpid(), getppid());
                    printf("---------------------------------------------\n");
                    break;
            }
            break;
        default:
            wait(NULL);
            new_pid2 = fork();
            switch(new_pid2){
                case -1:
                    printf("fork error\n");
                    break;
                case 0:
                    printf("\nI'm child process D.\n");
                    printf("Pid:%d\nParent pid:%d\n", getpid(), getppid());
                    printf("---------------------------------------------\n");
                    break;
                default:
                    wait(NULL);
                    new_pid3 = fork();
                    switch(new_pid3){
                        case -1:
                            printf("fork error\n");
                            break;
                        case 0:
                            printf("\nI'm child process E.\n");
                            printf("Pid:%d\nParent pid:%d\n", getpid(), getppid());
                            printf("---------------------------------------------\n");
                            break;
                        default:
                            wait(NULL);
                            printf("\nI'm child process A.\n");
                            printf("Pid:%d\nParent pid:%d\n", getpid(), getppid());
                            printf("---------------------------------------------\n");
                            break;
                    }
            }
            break;
    }
    return 0;
}
