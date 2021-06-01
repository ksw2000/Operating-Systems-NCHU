# CH6 Process Synchronization :closed_lock_with_key:

> Concurrent access to shared data may result in data inconsistency

## Race Condition :racehorse:

```nasm
; counter++
mov   eax, counter
inc   eax
mov   counter, eax
```

```nasm
mov   eax, counter
dec   eax
mov   counter, eax
```

according to scheduling results, these CPU instructions are executed in some arbitrary order.

| Thread1            | eax | Thread2            | eax | counter |
|:------------------:|:---:|:------------------:|:---:|:-------:|
| `mov eax, coutner` | 5   |                    |     | 5       |
| `inc eax`          | 6   |                    |     | 5       |
|                    |     | `mov eax, counter` | 5   | 5       |
|                    |     | `dec eax`          | 4   | 5       |
| `mov counter, eax` | 4   |                    |     | 4       |
|                    |     | `mov counter, eax` |     | 4       |

**Race condition:**

Several process concurrently access and manipulate the shared data.

**Solution:** Process synchronization and coordination: ensure only one process at a time can manipulate the shared data

## The Critical Section Problem :question:

+ *n* processes all competing to use some shared data

+ Each process has a code segment in which the shared data in accessed. -- This segment is called a **critical section (CS)**

+ Before entering the CS, each process must request the permission to ensure exclusive use of critical sections. The section is called **entry section**. Then, the critical section (CS) is followed by an **exit section**. The remaining code is the **remainder section**.

+ **Goal**
  
  > When one process is executing in its critical section, no other processes are executing in their critical sections at the same time

+ **Solution**
  
  > 1. Mutual Exclusion
  >    
  >    同一時間最多只有一個處理器處理 critical section
  > 
  > 2. Progress
  >    
  >    If no process is executing in its CS and there exists some processes that wish to enter their CS, only those processes that are not executing in their remainder sections can participate in deciding which process will enter its critical section next (不相干者不參與決策)
  > 
  > 3. Bounded Waiting
  >    
  >    After a process has made a request to enter its CS and before that request is granted, a bound must exists on the number of time that other processes are allowed to enter its critical section

## Peterson's Solution

1. Not guaranteed to work on modern architectures

2. **Two** processes solution

3. The load and store instructions must be atomic

```c
do{
    flag[i] = TRUE;
    turn = j;
    while(flag[j] && turn == j);
    // CRITICAL SECTION
    flag[i] = FALSE;
    // REMAINDER SECTION
}while(TRUE)
```

**cons:**

1. Prone to error if not careful

2. Not guarantee work on modern computer architecture
   
   > 編譯器因應效能，可能會將變數無相依的程式碼顛倒執行

3. A two processes solution
   
   > 僅適用於兩個 processes

4. Process that are requesting to enter their sections are busy waiting
   
   > If critical sections are long, it would be more efficient to block processes that are waiting

## Hardware Solution

> Rely on some special machine instruction

### memory barriers

aka. memory fences

> Enforce an ordering constraint on memory operations issued before and after the barrier instruction

e.g.

```c
// Thread 1
while(flag != true){
    memory_barrier();    // flag is loaded before x
}
print(x);

// Thread 2
x = 100;
memory_barrier();
flag = true;
```

Note that, memory barriers very considered very low-level operations -- only adopted by **kernel developers**.

### test\_and\_set solution

Definition:

```c
// this is cpu instruction code
// done by a single instruction
// and executing 
boolean test_and_set(boolean *target){
    boolean rv = *target
    *target = TRUE;
    return rv;
}
// return the original value of passes parameter
// set the new value of passed mparam in 
```

> shared boolean variable lock, initialized to false:
> 
> ```c
> do{
>     while(test_and_set(lock));
>     // CRITICAL SECTION
>     lock = false;
>     // REMAINDER SECTION
> }while(true);
> ```

lock = false

| Thread1                                                     | Thread2                                                     |
| ----------------------------------------------------------- | ----------------------------------------------------------- |
| rv = FALSE, *target = TRUE, return FALSE // *target is LOCK |                                                             |
| Enter CS, but soon context switch                           |                                                             |
| ---                                                         | rv = TRUE, *target = TRUE, return TRUE // *target is LOCK   |
| ---                                                         | rv = TRUE, *target = TRUE, return TRUE // *target is LOCK   |
| ---                                                         | rv = TRUE, *target = TRUE, return TRUE // *target is LOCK   |
| Exit CS                                                     |                                                             |
| LOCK = FALSE                                                |                                                             |
|                                                             | rv = FALSE, *target = TRUE, return FALSE // *target is LOCK |
|                                                             | Enter CS                                                    |

### compare\_and\_swap instruction

Definition:

```c
void compare_and_swap(int* value, int expected, int new_value){
    int temp = *value;
    if(*value == expected)
        *value = new_value;
    return temp;
}
```

```c
do{
    while(compare_and_swap(&lock, 0, 1) != 0);
    // CRITICAL SECTION
    lock = 0;
    // REMAINDER SECTION
}while(true);
```

| Thread1                             | Thread2                             |
| ----------------------------------- | ----------------------------------- |
| temp = *value = 0 // *value is lock |                                     |
| while() 不成立                         |                                     |
| CS, but soon context switch         |                                     |
|                                     | temp = *value = 1 // *value is lock |
|                                     | while() 成立                          |
|                                     | temp = *value = 1 // *value is lock |
| CS                                  | while() 成立                          |
| ...                                 | ...                                 |

### Bounded-waiting

Previous two versions (`test and set` and `compare and swap`) only guarantees **mut**ual **ex**clusion, but do not satisfy the bounded waiting requirement.

#### Bounded-waiting for test_and_set

```c
do{
    waiting[i] = TRUE;                           // ith P wish to enter CS
    while(waiting[i] && test_and_set(&lock));    // if waiting[i] = false || lock = false
    waiting[i] = FALSE;                          // ith P enters CS

    // CRITICAL SECTION

    j = (i+1)%n;                          // scanf from i+1, i+2, ... i-1
    while(j!=i && waiting[j] == FALSE)    // scan wait[j]
        j = (j+1)%n;

    if(j==i) lock = FALSE;                // nobody wants to enter CS
    else     waiting[j] = FALSE;

    // REMAINDER SECTION
}while(TRUE)
```

### Atomic variables :atom_symbol:

Typically, instructions such as `compare-and-swap` are used as building blocks for other synchronization tools. Not used directly to provide mutual exclusion.

One tool is `atomic varible`

1. Provides atomic updates on basic data types such as integers and booleans

2. x86 provides a `lock` prefix that tells the hardware: "don't let anyone read/write the value until I'm done with it"

**Example: Intel Lock Prefix**

```c
static inline int fetch_and_add(int* variable, int value){
    __asm__volatile("lock; xaddl *0, %1"
        : "+r"(value), "+m" (variable)    // input+output
        : // no input-only
        : "memory"
    )
    return value;
}
```

Adding `lock` prefix makes `xaddl` instruction executed atomically

**cons**

But, atomic variables do not entirely solve CS problems in all circumstances

+ only do a limited set of operations to basic data type. The lock prefix in x86 only works for some basic x86 instructions, e.g., `add`, `sub`, `and`, `xor`

+ Not enough to synthesize more complicated operations

> example1: what if we want to execute an arbitrary region of code without interference? e.g., red-black tree
> 
> example2: in the bounded-buffer problem. Assume one producer and two consumers, and buffer was empty. We use atomic integer for count and consider it's OK! When the producer entered an item in the buffer `count` was successfully set to 1. But both consumer could exit their while loop (as `count` would no longer be equal to 0) and proceed to consume the item => race condition is also occurred. (兩個顧客偵測到buffer為 1 時同時做拿取)

## Operation System Solution

previous hardware-based solutions are complicated and generally inaccessible to application programmers

OS build a more convenient approach to solve critical section problem

### Mutex Locks :lock:

Mutex (mutual exclusion) lock is a synchronization tool provided by the OS

+ mutex: a boolean variable

+ can only be accessed via two atomic (indivisible functions):
  
  > ```c
  > acquire(){
  >        while(mutex == FALSE);    // busy wait
  >        mutex = FALSE;
  > }
  > 
  > release(){
  >        mutex = TRUE;
  > }
  > 
  > /*
  >     These two functions must be implemented atomically.
  >     Both test-and-set and compare-and-swap can
  >     be used to implement these functions.
  > */
  > ```

+ Race condition problem solved by mutex
  
  > ```c
  > acquire(mutex);
  > // CRITICAL SECTION
  > release(mutex);
  > // REMAINDER SECTION
  > ```

---

> **Spin Locks**
> 
> previous mutex implementation needs **busy waiting**, this kind of lock is called a spinlock
> 
> busy waiting: 一直去檢查
> 
> spinlock is only useful in multi-processor systems

---

### Semaphore :signal_strength:

> A more powerful synchronization tool (than mutex) provided by OS

1. Counting semaphore
   
   > + unrestricted integer value
   > 
   > + control access to given resource with N instances
   > 
   > + Semaphore value is initialized to N

2. Binary semaphore
   
   > + integer value can range only between 0 and 1
   > 
   > + similar to mutex
   > 
   > + Binary semaphore is initialized to 1 

**implementation without busy waiting**

+ To avoid busy waiting, when a process has to wait, it will be **put in a waiting queue**

+ Each semaphore is associated with a waiting queue

+ Re-define a semaphore as a record
  
  ```c
  typedef struct{
      int value;
      struct procss *L; // PCB
  }semaphore;
  ```

```c
wait(S){
    S.value--;
    if(S.value < 0){
        // add this process to S.L
        // block
    }
}

signal(S){
    S.value++;
    if(S.value <= 0){
        // remove a process P from S.L;
        // wakeup(p)
    }
}
```

```c
//process A
wait(mutex);
// CS
signal(mutex);

//process B
wait(mutex);
// CS
signal(mutex);

//process C
wait(mutex);
// CS
signal(mutex);
```

+ signal(S) and wait(S) **should be atomic**
  Must guarantee that NO two processes can execute wait and signal() one the same semaphore at the same time
+ Solutions
  + Uniprocessor system
    Disable interrupts during the execution of signal() and wait() operations
  + Multicore system
    Disable interrupts does not work, typically compare-and-swap/text-and-set or spinlocks

### Monitor

**Problems with Semaphore**

Semaphores provide a convenient and effective mechanism for process synchronization. However, incorrect use may result in timing errors. The bugs are hard to detect.

Solution: high-level language constructs like **monitor**

> A high-level abstraction that provides a convenient mechanism for process synchronization

#### Monitors with condition variables

> **Questions:**
> 
> If A executes x.wait() and b is blocked, then B executes x.signal() that wakes up A
> 
> Now, *both B and A are active*
> 
> **Solve:**
> 
> 1. Signal and wait
> 
> 2. Signal and continue

## Liveness

A set of properties that a system must satisfy to ensure **processes make progress**

**Liveness failure** (Liveness hazards)

+ Fail to satisfy the liveness property

+ Result in poor performance and responsiveness

+ e.g. 
  
  > 1. Indefinite waiting
  > 
  > 2. Deadlock
  > 
  > 3. Priority Inversion

## Evaluation :chart_with_upwards_trend:

傳統上使用 lock-based scheme 來避免 race condition，但是做 lock 效能並不是很好，而且 race condition 並不是那麼容易發生，因此衍生出 lock-free programming，先執行，若有問題再做重新執行一次

```c
for(;;){
    // copy a shared variable (m_Head) to a local variable (oldHead)
    oldHead = m_Head;

    // Do the modification locally via newHead, thus, not yet visible to other threads
    newHead = m_Head;
    newHead->next = oldHead;

    // Next, attempt to publish our changes to the shared variable.
    // If the shared variable hasn't changed, the compare-and-swap succeed and we return
    // Otherwise, repeat
    if(compare_and_swap(&m_Head, oldHead, newHead) == oldHead) return;
}
```

1. **Atomic variables** are much lighter than mutex or semaphores for single updates to shared simple variables, e.g. integers

2. **Spinlock** are used on MP if CS is short

3. **Mutex locks** are simpler and less overhead than semaphores

4. However, if controlling access to a finite number of resources, **counter semaphores** is more appropriate than a mutex lock

5. **Read-write locks** are preferred over a mutex lock since it allows a higher degree of concurrency

6. **Monitor and condition variables** may have significant overhead and cannot scale well in highly contended (高度競爭) situations.
