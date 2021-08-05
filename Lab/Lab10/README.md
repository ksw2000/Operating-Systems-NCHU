# Lab10 Thread Local Storage & Thread Cancellation Points

## How to create thread-local variable

**Example1**
```c
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

static __thread int global = 0;

void* function(void* args){
  global++;
  printf("Thread id = %lu global = %d\n", pthread_self(), global);
  pthread_exit(NULL)
}

int main(){
  pthread_t id[2];
  pthread_create(id, NULL, function, NULL);
  pthread_create(id+1, NULL, function, NULL);
  pthread_join(id[0], NULL);
  pthread_join(id[1], NULL);
}
```

## Exercise 1 - local storage

Modify example code and implement "incr uncion", the value of "loops" parameter is passed by pthread_create()

```sh
gcc local_storage.c -o local_storage -pthread
```

## Exersice 2 cancellation points

When cancelability is enables and deferred, a cancellation request is acted upon only when a thread next reaches a cancellation point

The pthread_cancel() function sends a cancellation request to the specified thread

The pthread_setcancelstate(int state, int* oldstate) function sets the calling thread's cancelability state to the value given in state.

**Example2**
```c
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

void* child(void* arg){
  int* input = (int*) arg;
  int* result = malloc(sizeof(int));
  result[0] = input[0] + input[1];
  pthread_exit((void*)result);  // return result
}

int main(){
  pthread_t t;
  void* ret;
  int input[2] = {1, 2};

  pthread_create(&t, NULL, child, (void*)input);
  pthread_join(t, &ret);
  int* result = (int*)ret;
  printf("%d + %d = %d\n", input[0], input[1], result[0]);

  free(result);
  return 0;
}
```

Modify example2 code and observe the following situation:

+ When the thread has completely calculated, the main thread will send cancellation request to terminate the thread before it call pthread_exit()

+ Observe that the thread if system resource without appropriate release when suddenly terminate, the system resource will be recycled by OS or not?

```sh
gcc cancellation_point -o cancellation_point -pthread
```
