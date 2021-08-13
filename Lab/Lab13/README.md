# Lab13 Mutex and Condition Variable

## Statically Allocated Mutexes

```c
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
```

## Dynamically Allocated Mutexes

```c
int pthread_mutex_inti(pthread_mutex_t* mutex, const pthread_mutexattr_t* attr);

// return 0 if success
// the second parameter is used to specify the attributes of a mutex
```

```c
int s;
pthread_mutex_t mtx;
s = pthread_mutex_init(&mux, NULL);
```

## Condition Variable

Condition variable: signaling changes of state. 

Allow one thread to notify some other threads that some events have occurred. Without condition variables, the programmer would need to have threads continually polling to check if the condition is satisfied or not.

+ `pthread_cond_signal()` 
  + The function shall unblock at least one of the threads that are blocked on the specified condition variable *cond* (if any threads are blocked on *cond*). 
  + Reference: [https://linux.die.net/man/3/pthread_cond_signal](https://linux.die.net/man/3/pthread_cond_signal)
+ `pthread_cond_wait()`
  + Blocks a thread until the condition variable *cond* is signaled

## Exercise 1

+ We have three threads to run two different functions to access a same global variable.
    + Function 1: Addition  +2
    + Function 2: subtraction -2
+ Two thread execute Function 1 , one thread execute Function 2.
+ Each function execute ≥1000000 times. Show the result and check the result is correct or not. Ex: the result is 2000000 if 1000000 times

## Exercise 2

+ Print 1 3 5 infinitely using threads. 
+ Create three threads T1, T2, and T3 such that those should print 1 3 5 sequence infinitely by using condition variable)
+ (Hint1: Use three condition variable.)
+ (Hint2:change a glob variable value then print it.)
+ Output: 1 3 5 1 3 5 1 3 5 1 3 5 ...(loop)