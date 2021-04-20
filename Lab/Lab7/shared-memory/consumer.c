#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<unistd.h>
int main(){
    const char* name = "OS";
    int shm_fd;
    char* shm_base;
    char* ptr;
    char buffer[1024];
    shm_fd = shm_open(name, O_RDONLY, 0666);
    if(shm_fd<0){
        perror("open error:\n");
        exit(1);
    }
    shm_base = mmap(0, 1024, PROT_READ, MAP_SHARED, shm_fd, 0);
    if(shm_base == MAP_FAILED){
        perror("shm_base error\n");
        exit(1);
    }
    printf("%s", shm_base);
    return 0;
}
