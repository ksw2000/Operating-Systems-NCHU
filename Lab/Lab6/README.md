# Lab6 Process Control

## 1
Declare a global variable and a local variable. Both the global and local variables are initialized to the same value (=10).

Print both the values of global and local variables in screen with the process id and parent's id.

> global variable `g` is equal to local variable `l`

## 2

Write a program that creates 5 processes, forming a tree configuration illustrated in the figure. Each process print its process id and parent's process id.

```
    A
  / | \
  B D E
 /
C
```

Each process, if it is a parent, must make a number of `wait()` system calls to wait until all of it's children exit.
