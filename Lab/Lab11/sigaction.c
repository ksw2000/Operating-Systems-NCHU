#include<stdio.h>
#include<signal.h>
#include<unistd.h>

struct sigaction act;
struct sigaction alr;

void handler1(int sig_num){
    printf("How are you?\n");
}

void handler2(int sig_num){
    printf("I am fine!\n");
}

void change_handler(int sig_num){
    static int step = 0;
    if(step == 0){
        alarm(3);
        step++;
        act.sa_handler = handler2;
        sigaction(SIGINT, &act, NULL);
    }else{
        act.sa_handler = SIG_DFL;
        sigaction(SIGINT, &act, NULL);
    }
}

int main(){
    act = (struct sigaction){
        .sa_handler = handler1,
        .sa_flags = 0
    };
    sigemptyset(&act.sa_mask);

    /*
        unsigned alarm(unsigned secs)
        causes a SIGALRM signal to be sent to the calling process
        after a specified number of real secondes has elapsed
    */

    alr = (struct sigaction){
        .sa_handler = change_handler,
        .sa_flags = 0
    };
    sigemptyset(&alr.sa_mask);

    sigaction(SIGINT, &act, NULL);
    sigaction(SIGALRM, &alr, NULL);

    alarm(3);

    for(;;);
}
