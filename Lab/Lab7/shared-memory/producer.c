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
    // shm_open()
    // creates and opens a new shared memory object or opens an existing object
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if(shm_fd < 0){
        perror("open error:\n");
        exit(1);
    }
    // ftruncate()
    // set the size of a file to the value specified by length
    ftruncate(shm_fd, 1024);
    shm_base = mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_base == MAP_FAILED){
        perror("shm_base error:\n");
        exit(1);
    }
    fgets(buffer, sizeof(buffer), stdin);
    ptr  = shm_base;
    ptr += sprintf(ptr, "%s", buffer);
    // munmap()
    // removing a mapping from the calling process's virtual address space.
    if(munmap(shm_base, 1024) == -1){
        perror("munmap error:\n");
        exit(1);
    }

    if(close(shm_fd) == -1){
        perror("close error\n");
        exit(1);
    }
    return 0;
}
