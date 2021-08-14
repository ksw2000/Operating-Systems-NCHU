# Lab12 Semaphores

POSIX Semaphores allow processes and thread to synchronize access to shared resources.

## Exercise1

Using the unnamed semaphore to solve the race condition occurred below

```c
void inc(void){
  int i = 0;
  for(i=0; i<25000000; i++){
    count++;
  }
  pthread_exit(NULL);
}

void dec(void){
  int i = 0;
  for(i=0; i<25000000; i++){
    count--;
  }
  pthread_exit(NULL)
}
```

```sh
#compile
gcc unnamed_semaphores.c -pthread -o unnamed_semaphores
```

## Exercise2

Instead of use unnamed semaphore, use named semaphore to implement.

```sh
#compile
gcc named_semaphores.c -pthread -o named_semaphores
```
