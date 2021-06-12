# HW1

## Question
View powerpoint

## Answer

### Basic
前半部分主要是處理值的輸入，在這題中，我是採用一維陣列來處理二維矩陣
Matrix1 代表 c Marix2 代表 d Matrix3 則代表 result

```c
int n;
printf("matrix size:\n");
scanf("%d*%d", &n, &n);

int* matrix1 = malloc(sizeof(int) * n * n);
int* matrix2 = malloc(sizeof(int) * n * n);
int* matrix3 = malloc(sizeof(int) * n * n);

int i;
printf("matrix c (%d x %d)\n", n, n);
for(i=0; i<n*n; i++){
    scanf("%d", &matrix1[i]);
}

printf("matrix d (%d x %d)\n", n, n);
for(i=0; i<n*n; i++){
    scanf("%d", &matrix2[i]);
}
```

後半部的程式碼則是利用 lable + goto 的方式產生 n*n 個子進程，並且利用最後一個子進程的pid返推wait() 的值屬於哪一個子進程

```c
int fork_id, fork_count = n * n - 1;
fork:
    fork_id = fork();
    if(fork_id < 0){
        fprintf(stderr, "Fork() error\n");
        exit(EXIT_FAILURE);
    }else if(fork_id == 0){
        // fork_count: the nth element
        int row = fork_count / n;
        int col = fork_count % n;
        int sum = 0;
        for(i=0; i<n; i++){
            sum += matrix1[row*n+i] * matrix2[col+n*i];
        }
        exit(sum);
    }else{
        if(fork_count--) goto fork;

        // last child process id = fork_id;
        int get_sum;
        for(i=0; i<n*n; i++){
            pid_t child = wait(&get_sum);
            matrix3[fork_id - child] = WEXITSTATUS(get_sum);
        }

        printf("result:\n");
        for(i=0; i<n*n; i++){
            printf("%3d ", matrix3[i]);
            if(!((i+1) % n)) printf("\n");
        }
    }
}
```

```
matrix size:
3*3
matrix c (3 x 3)
1 2 3
4 5 6
7 8 9
matrix d (3 x 3)
9 8 7
6 5 4
3 2 1
result:
 30  24  18
 84  69  54
138 114  90
```

### Advance
因為現有的 share memory 用法太不親民了所以我先刻幾個high level 一點的 function 提供我使用。使用時就是先用 __Shm__ 建構一個新的 share memory ，然後利用 send 傳送訊息，利用 get 接收訊息，利用 close 關閉通道

```c
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
```

再來前面的部分幾乎跟 basic 一樣，主要就是從 stdin 獲取矩陣訊息

```c
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
```

接著利用lable + goto 的方式產生 n 個子進程，每個子進程會算出其那一row所得之值並利用自製的 send() 函式將值定位並傳給 share memory，最後印出時僅需從share memory 的 base 位移做位移即可得到完整的 result 矩陣

```c
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
```
