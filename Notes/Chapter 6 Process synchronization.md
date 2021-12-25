# CH6 Process Synchronization :closed_lock_with_key:

> Concurrent access to shared data may result in data inconsistency.

## Race Condition :racehorse:

Race condition: several process concurrently access and manipulate the shared data.

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

According to scheduling results, these CPU instructions are executed in some arbitrary order.

| Thread1            | eax | Thread2            | eax | counter |
|:------------------:|:---:|:------------------:|:---:|:-------:|
| `mov eax, coutner` | 5   |                    |     | 5       |
| `inc eax`          | 6   |                    |     | 5       |
|                    |     | `mov eax, counter` | 5   | 5       |
|                    |     | `dec eax`          | 4   | 5       |
| `mov counter, eax` | 4   |                    |     | 4       |
|                    |     | `mov counter, eax` |     | 4       |

**Solution:** Process synchronization and coordination: ensure only one process at a time can manipulate the shared data.

## The Critical Section Problem :question:

+ *n* processes all competing to use some shared data

+ Each process has a code segment in which the shared data in accessed. -- This segment is called a **critical section (CS)**

+ Before entering the CS, each process must request the permission to ensure exclusive use of critical sections. The section is called **entry section**. Then, the critical section (CS) is followed by an **exit section**. The remaining code is the **remainder section**.

  ```c
  while(true){
      // entry section (start)
      	// critical section (shared data is accessed here)
  	// exit section (end)
      	// remainder section (remaining code)
  }
  ```

+ **Goal**

  > When one process is executing in its critical section, no other processes are executing in their critical sections at the same time

+ **Solution**

  1. **Mutual exclusion**: If a process P executing in its critical section, then no other processes can be executing in their critical sections. 同時間最多只有一個處理器在處理 critical section
  2. **Progress**: If no process is executing in its critical section and there exists some processes that wish to enter their critical section, only those processes that are not executing in their remainder sections can participate in deciding which process will enter its critical section next. 不相干者 (在remainder section者) 不得參與誰能進 critical section 的決策
  3. **Bounded waiting**: There exists a bound, or limit, on the number of times that other processes are allowed to enter their critical sections after a process has made a request to enter its critical section and before that request is granted.

Two general approaches are used to handle critical sections in OS: preemptive kernels and nonpreemptive kernels.

1. A preemptive kernel allows a process to be preempted while it is running in kernel mode.
2. A nonpreemptive kernel does not allow a process running in kernel mode to be preempted; a kernel-mode process will run until it exits kernel mode, blocks, or voluntarily yields control of the CPU.

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

As discussed, software-based solutions are not guaranteed to work on modern computer architectures. In this section, we present 3 hardware instructions that provide support for solving the critical-section problem.

### 1. Memory barriers

(aka. memory fences)

> Enforce an ordering constraint on memory operations issued before and after the barrier instruction

1. **Strong ordered**, where a memory modification on one processor is **immediately visible to all other processors**.
2. **Weakly ordered,** where modifications to memory on one processor may **not be immediately visible to other processors**.

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

When a memory barrier instruction is performed, the system ensures that all loads and stores are completed before any subsequent load or store operations are performed. Therefore, even if instructions were reordered, the memory barrier ensures that the store operations are completed in memory and visible to other processors before future load or store operations are performed.

Note that, memory barriers very considered very low-level operations -- only adopted by **kernel developers**.

### 2. Hardware Instruction

#### 2.1 test\_and\_set solution

Many modern computer systems provide special hardware instructions that allow us either to test and modify the content of a word or to swap the contents of two words **atomically**. If the machine supports the `test_and_set()` instruction, then we can implement mutual exclusion by declaring a boolean variable `lock`, initialized to `false`.

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

#### 2.2 compare\_and\_swap instruction

The `compare_and_swap()` instruction, just like the `test_and_set()` instruction, operates on two words atomically, but uses a different mechanism that is based on swapping the content of two words.

Definition:

```c
int compare_and_swap(int* value, int expected, int new_value){
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

> On Intel x86 architectures, the assembly language statement `cmpxchg` is used to implement the `compare_and_swap()` instruction. To enforce atomic execution, the `lock` prefix is used to lock the bus while the destination operand is being updated. The general form of this instruction appears as:
>
> ```nasm
> lock cmpxchg <dest operand> <src operand>
> ```
>
> 

#### Note: Bounded-waiting problem

Previous two versions (`test and set` and `compare and swap`) only guarantees **mut**ual **ex**clusion, but do not satisfy the bounded waiting requirement.

**Bounded-waiting for test_and_set()**

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

### 3. Atomic variables :atom_symbol:

Typically, instructions such as `compare-and-swap()` are used as building blocks for other synchronization tools. Not used directly to provide mutual exclusion. One tool is `atomic varible`

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

Previous hardware-based solutions are complicated and generally inaccessible to application programmers. OS build a more convenient approach to solve critical section problem

### 1. Mutex Locks :lock:

The mutex we introduced here is a spin lock.

Mutex (mutual exclusion) lock is a synchronization tool provided by the OS

Mutex, a boolean variable, can only be accessed via two atomic (indivisible functions):

> ```c
> acquire(){
>     while(mutex == FALSE);    // busy wait
>     mutex = FALSE;
> }
> 
> release(){
>     mutex = TRUE;
> }
> 
> /*
>  These two functions must be implemented atomically.
>  Both test-and-set and compare-and-swap can
>  be used to implement these functions.
> */
> ```
>
> **Race condition problem solved by mutex**
>
> ```c
> acquire(mutex);
> // CRITICAL SECTION
> release(mutex);
> // REMAINDER SECTION
> ```
>
> 

Locks are either **contended** (爭執中的) or **uncontended**. A lock is considered contended if a thread blocks while trying to acquire the lock. If a lock is available when a thread attempts to acquire it, the lock is considered uncontended. Contended locks can experience either high **contention** (a relatively large number of threads attempting to acquire the lock) or low contention. Highly contended locks tend to decrease overall performance of concurrent applications.

Spin locks: previous mutex implementation needs **busy waiting**, this kind of lock is called a spinlock. Because the process "spins" while waiting for the lock to become available. spinlock is only useful in multi-processor system.

---

### 2. Semaphore :signal_strength:

A more powerful synchronization tool (than mutex) provided by OS. A semaphore is an integer variable that, apart from initialization, is accessed only through two standard atomic operations: `wait()` and `signal()`. See [Lab12](https://github.com/liao2000/Operating-Systems-NCHU/tree/main/Lab/Lab12)

1. Counting semaphore
   
   + unrestricted integer value
   + control access to given resource with N instances
   + Semaphore value is initialized to N (when calling `wait()`, decrease the count if counter is non-zero. when count goes to 0, all block until the count becomes greater than 0)
2. Binary semaphore
   + integer value can range only between 0 and 1
   + similar to mutex
   + Binary semaphore is initialized to 1 

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
        // sleep() 告訴 CPU 把這個 Thread 移入 waiting state
    }
}

signal(S){
    S.value++;
    if(S.value <= 0){
        // remove a process P from S.L;
        // wakeup(P) 告訴 CPU 把 P 移入 ready state
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

## Monitor

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

**Liveness** refers to a set of properties that a system must satisfy to ensure that processes make progress during their execution life cycle. A process waiting indefinitely under the circumstances just described is an example of a "liveness failure" (aka liveness hazards).

+ Fail to satisfy the liveness property
+ Result in poor performance and responsiveness
+ e.g.
  
  1. Indefinite waiting
  2. Deadlock
  3. Priority Inversion

#### Priority inversion

Assume we have 3 processes: L, M and H, whose priorities follow the order L < M < H. Assume that process H requires a semaphore S, which is currently being accessed by process L, Ordinarily, process H would wait for L to finish using resource S. However, now suppose that process M becomes runnable, thereby preempting process L. Indirectly, a process with lower priority, process M, has affected how long process H must wait for L to relinquish (放棄) resource S. (也就是說變成由 M 決定 H 是否可以繼續執行，整個優先序顛倒了)

This liveness problem can be avoided by implementing a **priority-inheritance protocol**. All processes that are accessing resources needed by a higher-priority process inherit the higher priority until they are finished with the resources in question. When they are finished, their priorities revert to their original values.

當 H 存取 L 時將 L 的優先序暫時提高，使得 L 不會被 M 綁住，當 L 執行完後將資源放棄時再由 H 接收，如此以來就不會發生 priority inversion 的問題

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
