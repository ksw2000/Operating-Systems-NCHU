#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
int main(){
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
