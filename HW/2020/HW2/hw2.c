#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
int *matrix1, *matrix2, *matrix3;
int n;
void* mulArr(void* r){
	int i, j;
	int row = *((int*)r);
	for(i=0; i<n; i++){
		// row = *(int*)r , col = i
		for(j=0; j<n; j++){
			matrix3[row * n + i] += matrix1[row * n + j] * matrix2[j*n + i];
		}
	}
}

int main(){
	int i;
	printf("matrix size:\n");
	scanf("%d*%d", &n, &n);

	matrix1 = malloc(sizeof(int) * n * n);
	matrix2 = malloc(sizeof(int) * n * n);
	matrix3 = calloc(n*n, sizeof(int));

	printf("matrix a\n");
	for(i=0; i<n*n; i++) scanf("%d", &matrix1[i]);
	printf("matrix b\n");
	for(i=0; i<n*n; i++) scanf("%d", &matrix2[i]);
	printf("matrix c:\n");

	pthread_t* thread_arr = malloc(sizeof(pthread_t) * n);
	int* args = malloc(sizeof(int)*n);
	for(i=0; i<n; i++){
		args[i] = i;
		pthread_create(thread_arr+i, NULL, mulArr, args+i);
	}

	for(i=0; i<n; i++){
		pthread_join(thread_arr[i], NULL);
	}

	for(i=0; i<n*n; ){
		printf("%4d ", matrix3[i]);
		if(!(++i % n)){
			printf("\n");
		}
	}
}

