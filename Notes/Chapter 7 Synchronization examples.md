# CH7 Synchronization Examples

## Classic problems of synchronization

### Bounded-buffer problem

N buffers, each can hold one item

+ Producer: produces full buffers, waits for empty buffers
+ Consumer: produces empty buffer, waits for full buffers

**producer**

```c
do{
    // produce an item in nextp (p means produce)
    wait(empty);
    wait(mutex);
    // ...
    // add nextp to buffer
    // ...
    signal(mutex);
    signal(full);
}while(1);
```

**consumer**

```c
do{
    wait(full);
    wait(mutex);
    // ...
    // remove an item from buffer to nextc (c means consume)
    // ...
    signal(mutex);
    signal(empty);
    // consume the item in nextc
}while(1);
```

### Readers :eyes: and Writers :pencil2: Problem

A data set is shared among a number of concurrent

+ Readers: only read the data set; they do not perform any updates

+ Writers: can both read and write.

Goal

+ Allow multiple readers to read at the same time

+ But only one single writer can access the shared data at the same time.

**Writer process**

```c
while(true){
    wait(rw_mutex);

    /*writing is performed*/

    signal(rw_mutex)
}
```

**Reader process**

```c
while(true){
    wait(mutex);
    read_count++;
    if(read_count == 1)
        wait(rw_mutex);
    signal(mutex);

    /*reaing is performed*/

    wait(mutex)
    read_count--;
    if(read_count == 0){
        signal(rw_mutex);
    }
    signal(mutex);
}
```

### Dining-Philosophers Problem :fork_and_knife:

Five philosophers who only eat and think. They need 2 forks ​ for eating but we have only 5 forks

**Solution 1: using semaphore**

```c
do{
    wait(fork[i]);
    wait(fork[i+1]%5);
    // eat
    signal(fork[i]);
    signal(fork[i+1]%5);
    // think
}while(1);
```

> **WILL CAUSE DEADLOCK** :lock:
> 
> All philosophers become hungry simultaneously, and each grabs her left fork.
> 
> i.e., each philosopher executes only the first wait() statement and then context switch.
> 
> DEADLCOK Solution
> 
> Pick up forks only if both forks are available

**Solution 2: using monitor**

```c
enum{
    THINKING, HUNGRY, EATING
}state[5];

struct condition self[5];

void pickup(int i){
    state[i] = HUNGRY;
    try_to_eat(i);
    if(state[i] != EATING){
        // if try_to_eat fail, waite
        self[i].wait();
    }
}

void putdown(int i){
    state[i] = THINKING;
    try_to_eat((i+4)%5);    // test previous one
    try_to_eat((i+1)%5);    // test next one
}

void try_to_eat(int i){    // try to eat
    if((state[(i+4) % 5] != EATING) 
       && state[i] == HUNGRY
       && state[(i+1) % 5 != EATING]){
        state[i] = EATING;
        self[i].signal();
    }
}

void init(){
    int i;
    for(i=0; i<5; i++){
        state[i] = THINKING;
    }
}
```
