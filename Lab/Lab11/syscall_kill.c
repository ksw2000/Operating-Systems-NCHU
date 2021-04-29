#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
int main(){
    pid_t p;
    p = fork();
    if(p<0){
        fprintf(stderr, "fork error");
        exit(EXIT_FAILURE);
    }else if(p == 0){
        printf("child process\n");
        printf("child pid = %d\n", getpid());
        kill(getppid(), SIGTERM);
        kill(getpid(), SIGTERM);
        for(;;);
    }else{
        printf("parent process\n");
        printf("parent pid = %d\n", getpid());
        for(;;);
    }
    return 0;
}
