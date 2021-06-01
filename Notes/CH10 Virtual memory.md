# CH10 Virtual memory

**motivation** :bulb:

In many cases, entire program is not needed. Even when the entire program is needed, it may not all needed at the same time

Thus, consider ability to execute partially-loaded program.

If we use virtual memory we can

1. **Less memory needed.** Program no longer constrained by sizes of physical memory

2. **More processes in memory** since each program takes less memory

3. **Less I/O needed**. Do not need to load complete program

4. **Faster response**. Less I/O results in faster response

## Demand Paging

Bring a page from disk into memory only when it is needed.

Comparison

+ swapper: manipulate entire processes
+ pager (or Lazy swapper): manipulate the individual pages of a process

![](./src/10-1.png)

where to put the pages that are paged out -- Secondary memory, specifically, a partition on the secondary memory called **swap space** or **backing store**

### Hardware support for demand paging

How do we know if a page is in memory or not? Need hardware support. Valid-Invalid bit in the page table. The valid-invalid bit is set by OS.

During address translation by MMU, if valid-invalid bit is 0 → CPU signals page-fault trap to OS

OS look at another table P<sub>i</sub> to decide

+ Invalid reference: terminate the process

+ Just not in memory: page in

![](./src/10-2.png)

Why demand paging is useful i.e., without causing many page faults

Because of locality of reference

## The fork() system call :fork_and_knife:

fork() creates a copy of the parent's address space for child, but, the copying may be unnecessary.

### Copy-on-Write

But, if child process invoke exec()? Cannot share anymore

+ Parent and child initially shares the same page frames
+ Pages that can be modified are marked as copy-on-write if either process writes to a copy-on-write page

## Page Replacement

一份資料在記憶體一份在硬碟

use modify bit (or dirty bit)

set by the hardware if a frame page is modified

reduce overhead -- only modified pages are written to disk

if m = 1, write back to disk

if m = 0, directly replace the page

**which frame is to be replaced?**

### FIFO Page Replacement

可能買越多記憶體反而要花更多次去硬碟搬資料

### Optimal algorithm

Replace page that will not be used for longest period of time? But how to implement?

#### Least Recently Used (LRU)



### Reference bit



---

dirty bit: 有寫的時候設為 1 ，表示記憶體與硬碟中的值不一樣

reference bit: OS 定期將期清為 0 ，有使用時設為 1，可提供給 Swap out 需要取代時做選擇


