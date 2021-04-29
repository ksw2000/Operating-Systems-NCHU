# Lab9 Thread Management

## POSIX Thread

Using pthreads to create 4 identical pthreads. Besides, declare a global variable `count`, where count = 0.

The main program passes a value containing the number of iterfarions to the threads.

Each thread increments the same global variable 250,000 times.

>
> Do not need to use join() in this example
> The main thread may finish before children's threads
>

**compile**

```sh
gcc pthread.c -o pthread -pthread
```

## Java Thread

Using Java thread to create 4 identical java threads like previous lab. However we use `join()` in this example

**compile and run**

```sh
javac Jthread.java  # compile
java Jthread        # run
```

## Thread with message queue

Create 2 programs, each of them has 2 thread by using the message queue which we learned at Lab7. One thread can allow user type messages iteratively and send those messages to a queue. The other one can receive the messages which be sent to the queue by the other proces and print on screen.

The 2 processes use the same message queue, but assign different message type to distinguish each other.

**compile**
```sh
gcc sender.c -o sender -lrt -pthread
gcc receiver.c -o receiver -lrt -pthread
```
