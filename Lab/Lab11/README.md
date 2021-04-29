# Signals

## Exercise1

1. Using the fork() function to create a new process
2. Using terminal to kill the process.
3. Repeat step1 and use system calls to kill process.

```sh
gcc syscall_kill.c -o syscall_kill -lrt
```

## Exercise2

1. Writing handler1: show "How are you?" message.
2. Writing handler2: show "I am fine!" message
3. In the first 3 secondes, you have to maintain handler1 as your SIGINT handler.
4. Next 3 seconds, you have to maintain handler2 as your SIGINT handler
5. Then you set SIGINT handler to default handler

```sh
# compile
gcc sigaction.c -o sigaction
# run
./sigaction
# press Ctrl + C periodly
# The first 3 secondes get "How are you?" on stdout
# The next 3  secondes get "I am fine!" on stduout
```
