# Lab7 Message Queue and Shared Memory

Inter-process communication (IPC) practice

## Message Queue

1. Implement a sender and a receiver.
2. When sender send `exit`, close message queue.

**compile**
```c
gcc sender.c -o sender -lrt
gcc receiver.c -o receiver -lrt
```

## Shared Memory

1. Implement a producer and a consumer.
2. Firstly, execute producer and type some data on stdin.
3. Secondly, execute a consumer, and it will show the text you typed at producer.

**compile**
```c
gcc producer.c -o producer -lrt
gcc consumer.c -o consumer -lrt
```
