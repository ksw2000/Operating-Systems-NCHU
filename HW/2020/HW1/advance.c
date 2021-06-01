#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<sys/wait.h>
#include<unistd.h>
typedef struct shm{
    int fd;
    const char* name;
    void* base;
    off_t size;
    void (*send)(struct shm* this, void* content, int start, size_t bytes);
    void* (*get)(struct shm* this);
    void (*close)(struct shm* this);
}Shm;

void Shm_send(Shm* this, void* content, int start, size_t bytes){
    memcpy((this->base) + start, content, bytes);
}

void* Shm_get(Shm* this){
    return this->base;
}

void Shm_close(Shm* this){
    munmap(this->base, this->size);
    close(this->fd);
    shm_unlink(this->name);
    free(this);
}

Shm* __Shm__(const char* name, off_t size){
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if(shm_fd < 0) return NULL;
    ftruncate(shm_fd, size);
    void* shm_base = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    Shm* new = malloc(sizeof(Shm));
    new->fd = shm_fd;
    new->name = name;
    new->base = shm_base;
    new->size = size;
    new->send = Shm_send;
    new->get = Shm_get;
    new->close = Shm_close;
    return new;
}

int main(){
    int n;
    printf("matrix size: ");
    scanf("%d*%d", &n, &n);
    int* matrix1 = malloc(sizeof(int) * n * n);
    int* matrix2 = malloc(sizeof(int) * n * n);
    int* matrix3 = malloc(sizeof(int) * n * n);
    int i, j;
    printf("matrix c (%d x %d)\n", n, n);
    for(i=0; i<n*n; i++) scanf("%d", &matrix1[i]);
    printf("matrix d (%d x %d)\n", n, n);
    for(i=0; i<n*n; i++) scanf("%d", &matrix2[i]);

    int id, fork_count = n - 1;
    // init my Shm
    Shm* share_mem = __Shm__("sahre_memory", sizeof(int) * n * n);

fork:
    id = fork();
    if(id < 0){
        fprintf(stderr, "Fork error!\n");
        exit(EXIT_FAILURE);
    }else if(id == 0){
        int* tmp = calloc(n, sizeof(int));
        for(i=0; i<n; i++){
	        for(j=0; j<n; j++){
                // fork_count-th row of C
                // i-th col of D
                tmp[i] += matrix1[fork_count*n+j] * matrix2[i+n*j];
            }
        }
        // send to share memory
        // share_mem(this, content, start, bytes)
        share_mem->send(share_mem, tmp, n * fork_count * sizeof(int), sizeof(int) * n);
    }else{
        if(fork_count--) goto fork;
        for(i = 0; i<n; i++) wait(NULL);
        printf("result:\n");
        void* base = share_mem->get(share_mem);
        for(i = 0; i<n*n;){
            printf("%d ", *((int*)(base) + i));
            if(++i % n == 0) printf("\n");
        }
    }
    share_mem->close(share_mem);
}
