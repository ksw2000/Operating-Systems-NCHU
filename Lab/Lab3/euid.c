#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
int main(){
    /*
        getuid():  get the real user ID
        geteuid(): get the effective ID
    */
    printf("%d\n", getuid());
    printf("%d\n", geteuid());
}
