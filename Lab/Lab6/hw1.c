#include<stdio.h>
#include<sys/wait.h>
#include<unistd.h>

int g = 10;

int main(){
    pid_t new_pid;
    int l = 10;
    new_pid = fork();
    switch(new_pid){
        case -1:
            printf("fork error\n");
            break;
        case 0:
            printf("\n------------This is child process------------\n");
            printf("Pid:%d\nParent pid:%d\nglobal:%d\nlocal:%d\n\n", getpid(), getppid(), g, l);
            printf("---------------------------------------------\n");
            break;
        default:
            wait(NULL);
            printf("\n------------This is parent process-----------\n");
            printf("Pid:%d\nParent pid:%d\nglobal:%d\nlocal:%d\n\n", getpid(), getppid(), g, l);
            printf("---------------------------------------------\n");
    }

    g++;
    l++;

    switch(new_pid){
        case -1:
            printf("fork error\n");
            break;
        case 0:
            printf("\n------------This is child process------------\n");
            printf("Pid:%d\nParent pid:%d\nglobal:%d\nlocal:%d\n\n", getpid(), getppid(), g, l);
            printf("---------------------------------------------\n");
            break;
        default:
            wait(NULL);
            printf("\n------------This is parent process-----------\n");
            printf("Pid:%d\nParent pid:%d\nglobal:%d\nlocal:%d\n\n", getpid(), getppid(), g, l);
            printf("---------------------------------------------\n");
    }
    return 0;
}
