# CH4 Threads & Concurrency

## Benefits of multi-thread

1. Responsiveness
   + 如果有一個執行緒卡住，其他執行緒仍能執行
2. Resource sharing
   + 不像多行程, 多執行緒的 code 區和 data 區是共有的
3. Economy
   + 創建一個執行緒是創建一個行程的30倍快，且執行緒的 context-switch 是多行程的5倍快，
4. Scalability
   + Multithreading on a multi-CPU machine increases concurrency
   + Each thread may be running in parallel

> *Q: What are benefits of threads?*
> *A: Responsiveness, Resource Sharing, Economy, Scalability*

> *Q: What sections and resources are shared between threads belonging to the
> same process?*
> *A: code section, data section, other OS resources (ex: open file & signals)*

## Concurrency(並行) vs Parallelism(平行)

On a single-processor systems, processes were running concurrently, but not in parallel

![](https://i.imgur.com/YaUHsgp.png)

## Types of Parallelism

Data Parallelism and Task Parallelism

![](https://i.imgur.com/nVufBG0.png)

## Types of Thread

1. User thread: provided at the user level by **user-level library**
   
   > Drawback:
   > 
   > 1. Blocking problem
   >     Entire process will block if a thread makes a blocking system call
   > 2. No support for running in parallel on multicore systems.
   >     Since the kernel only sees a single thread. 

2. Kernel thread: provided at the kernel (OS) 
   
   > Creating a user thread requires creating the corresponding kernel thread
   > 
   > 1. Overhead
   > 2. To bound the overhead, OS may restrict the number of threads supported

把 kernel 視為房東，把 user 視為房客。

1. user thread 就好比房客私底下協調讓一間公寓有兩個人住而房東不知道
2. kernel thread 就好房東知道這間公寓要給兩個人住，房東就必需協調資源有效分配

所以創建一個 user thread 比 kernel thread 省時

## Multithreading Models

+ Many-to-One Model
  
  + Many user-level threads mapped to single kernel thread.
  + No support for running in parallel on multicore systems since the kernel only sees a single thread. i.e., true concurrency is not gained

+ One-to-One Model
  
  + Each user-level thread maps to a kernel thread
  + Creating a user thread requires creating the corresponding kernel thread, and a large number of kernel threads may burden the performance of a system. i.e., cannot create too many threads

+ Many-to-Many Model
  
  + Many-to-many = many-to-one + one-to-many (two-level model)
  + Allows M user-level thread to be mapped to N kernel threads, where N $\le$ M
  + It is difficult to implement

> *Q: What is the blocking problem in many-to-one thread model?*
> *A: Entire process (All threads) will block if a thread makes a blocking system call.*

> *Q: Why the many-to-one thread model suffers from the blocking problem?*
> *A: Kernel only sees a single thread.*

## Thread Libraries

Three main thread libraries are in use today: POSIX Pthreads, Windows, and Java.

1. Pthreads, be provided as either a user-level or kernel-level library.
2. Windows thread library is a kernel-level library available on Windows system.
3. Java thread API allows thread to be created an managed directly in Java programs.

## Strategies for Creating Threads

1. Asynchronous threading
   
   > After creating children, parent resumes its execution
   > Parent and child run concurrently
   > e.g., a multithreaded server

2. Synchronous threading

> After creating children, parent wait for all children to terminate
> called fork-join strategy
> e.g., array summation (各個執行緒算一部分，算完後由主執行緒彙整)

## Implicit Threading

設計多執行緒程式並沒有想像中的簡單，因此有許多方法可以解決這個困難，那就是把這些處理執行緒的步驟包裝給 compilers 以及 run-times libraries

### 1. Thread pools

Commonly used in web servers. Thread pools work well when the tasks submitted to the pool can be executed asynchronously.

1. creating a number of threads in a pool to wait for working
2. receiving a request →  awaken a thread from the pool → pass it the request to service
3. once the thread completes its service → return to the pool
4. if no available thread → server waits until one become free

pros:

1. Faster to service a request with an existing thread than create a new thread
2. allows the number of threads in the apps to be bound to the size of the pool
3. <u>Separating task to be performed from mechanics of creating tasks</u>  allows different strategies for running task

The number of threads in the pool can be set heuristically based on factors such as the number of CPUs in the systems, and the expected number of concurrent client requests. More sophisticated thread pool architectures can dynamically adjust the number of threads in the pool according to usage patterns.

### 2. Fork Join

Divide-and-conquer.

```java
import java.util.concurrent.*
public class SumTask extends RecursiveTask<Integer>{
    static final int THRESHOLD = 1000;
    private int begin, end;
    private int[] array;
    public SumTask(int begin, int end, int[] array){
        this.begin = begin;
        this.end = end;
        this.array = array;
    }

    protected Integer compute(){
        if(end-begin<THREADSHOLD){
            // TODO
        }else{
            int mid = (begin+end)/2;
            SumTask leftTask = new SumTask(begin, mid, array);
            SumTash rightTash = new SumTask(mid+1, end, array);

            leftTask.fork();
            rightTask.fork();

            return rightTask.join() + leftTask.join();
        }
    }
}
```

### 3. OpenMP

OpenMP is a set of compiler directives as well as an API for programs written in C, C++, or FORTRAN that provides support for parallel programming in shared-memory environments.

```C
#include<omp.h>
#include<stdio.h>
int main(){
    #pargma omp parallel{
        printf("I am a paralledl region");
    }
    #pragma omp parallel for{
        for(i=0; i<N; i++){
            c[i] = a[i]+b[i];
        }
    }
}
```

### 4. Grand Central Dispatch (GCD)

GCD is a technology developed by Apple for its macOS and iOS.

### 5. Intel Thread Building Blocks (TBB)

Intel Thread Building Blocks is a template library that supports designing parallel applications in C++.

## Threading issue

### The fork() and exec() System Calls

Semantics of `fork()` and `exec()`.

If a thread in a program calls `fork()`, does the new process

1. duplicate all threads
2. duplicate only the thread that invoked fork()

> Some UNIX system supports tow versions of fork()

As for `exec()`, if a thread invokes the exec system call, the program specified in the parameter to `exec()` will replace the entire process - including all threads. Because threads share the `.code` section.

Which of 2 versions of `fock()` to use depends on the application. If `exec()` is called immediately after forking, then duplicating all threads is the unnecessary. In this instance, duplicating only the calling thread is appropriate. Otherwise, the separate process process should duplicate all threads.

### Signal Handling

Signal are used in UNIX systems to notify a process that a particular event has occurred. There are 2 types of signal, either synchronous signals or asynchronous signals. 

1. synchronous signals
   
   > Delivered to the same process that causes the signal.
   > e.g., illegal memory access, divided by zero, ... etc

2. asynchronous signals
   
   > Generated by external events
   > e.g., `ctrl-c` to terminate a process, timer expires, ... etc

Whether synchronous or asynchronous, follow the same pattern:

1. signal is generated by a particular event
2. signal is delivered to a process
3. signal is handled by the signal handler(s). Default signal handler in the kernel and user-defined signal handler

Handling signals in single-threaded programs is straightforward. However, delivering signals is more complicated in multithreaded programs, where a process may have several threads.

1. Deliver the signal to the thread to which the signal applies.
2. Deliver the signal to every thread in the process.
3. Deliver the signal to certain threads in the process.
4. Assign a specific thread to receive all signals for process.

Answer: Depends on the type of signal generated. e.g., `ctrl-c` should be sent to all threads.

See [Lab11](https://github.com/liao2000/Operating-Systems-NCHU/tree/main/Lab/Lab11)

Although Windows does not explicitly provide support for signals, it allows us to emulate them using **asynchronous procedure calls** (APCs). APC is roughly equivalent to an asynchronous signal in UNIX. However, whereas UNIX must contend with how to deal with signals in a multithreaded environment, the APC facility is more straightforward, since an APC is delivered to a particular thread rather than a process.

### Thread Cancellation

Terminate a thread before it has completed. There are 2 approaches for thread cancellation

1. asynchronous cancellation
   + terminates the target immediately
   + 有可能會出事，因為 resource reclaim
     + the target thread is holding resources
     + the target thread is in the midst of updating a shared data
2. deferred cancellation
   + the target thread periodically check if it should terminate
   + allow the target thread to terminate itself in a safe pointer, called **cancellation point**. See [Lab10](https://github.com/liao2000/Operating-Systems-NCHU/tree/main/Lab/Lab10)

The difficulty with cancellation occurs in situations where resources have been allocated to a cancelled thread or where a thread is cancelled while in the midst of updating data it is sharing with other threads.

```c
pthread_t tid;
pthread_create(&tid, NULL, worker, NULL);
//...
pthread_cancel(tid);
pthread_join(tid, NULL);
```

Asynchronous cancellation is not recommended in Pthreads documentation. On Linux systems, thread cancellation using the Pthreads API is handled through signals. In Java:

```java
Thread worker;
// ...
worker.interrupt()
```

### Thread-Local Storage

Thread belonging to a process share the data of the process. In some circumstances, each thread might need its own copy of certain data. We call such data **thread-local storage** or (**TLS**).

1. In Java, use `ThreadLocal<T>` class with `set()` and `get()`
2. In Pthreads, use `pthread_key_t` witch provides a key that is specific to each thread.
3. In C#, use `ThreadStatic` to declare thread-local data
4. In gcc compiler, use `__thread` for declaring TLS data. See [Lab10](https://github.com/liao2000/Operating-Systems-NCHU/tree/main/Lab/Lab10)

### Scheduler Activations

// TODO