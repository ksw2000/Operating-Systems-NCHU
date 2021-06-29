# I/O Systems

## I/O Hardware

### Bus

+ PCIe (Peripheral Component Interconnect Express): Common PC System bus
+ I/O bus: connects relative slow devices such as the keyboard, serial port, and USB ports

### Controller

controller can be a chip or an expansion card. Expansion card aka **host bus adapter**

+ disk controller: disk arm moving scheduling, prefetching, back-sector mapping, buffering/caching
+ bus controller

### Internal structure of a device

+ very simple devices: keyboard

+ more complex devices: include a simple CPU, memory and other device-specific chip.
  + **Firmware(韌體)** running on the CPU
  + e.g., SSD controller, 無線網路晶片

### How do CPU give commands and data to controllers

Not only CPU, controllers also have registers.

**Registers** act as an interface between **software** and **I/O hardware**

## A Simplified I/O Hardware

The simplified device interface is comprised of 3 registers

+ **status reg**: can be read to see the status of the device
+ **command reg:** tell the device to perform a certain task
+ **data reg**: pass data to device, or get data from the device

Sample OS code to get the device to do something on behave of process

```c
while(STATUS == BUSY);
for(i:=0; i< data_length; i++){
    write_data_to_DATA_reg();
    write_command_to_COMMAND_reg();
}
while(STATUS == BUSY);
```

However, there are some problems in the above code?

*inefficiencies*

+ **polling** (busy waiting)
+ **programmed I/O (PIO)** (請CPU搬資料很浪費，CPU應該要做其他更困難的事)

*solution*

+ 利用 interrupt 代替 busy waiting，讓 CPU 在這段時間可以去做其他事情
+ 利用 DMA (direct memory access) 來搬資料，而 CPU 只需把記憶體寫入硬碟的 data reg

**How to communicate with device**, i.e., how to read/write these device registers?

Sol. assign each device register a number, or ID, or address. However, how to decide the number

*Port-mapped I/O*

| I/O address | device               |
| ----------- | -------------------- |
| 000 - 00F   | DMA controller       |
| 020 - 021   | interrupt controller |
| 040 - 043   | timer                |
| ...         | ...                  |

> 
> ```assembly
> mov eax, 0
> ```
> 
> 當使用此指令時 CPU 不知道 0 是指記憶體的 0 還是 port-mapped 的 0，所以**要有額外的指令集**

*Memory-mapped I/O*

直接把硬碟的暫存器 mapping 到記憶體上，x86 理論上能使用 mapping 到 $2^{48}$，但實際上並不會用到那麼多。該方法主要應用在行動裝置 Arm 的 CPU

## Interrupt

How CPU knows an interrupt occurs? CPU has a wire called interrupt-request line.

CPU jumps to the interrupt-handler routing at a fixed address in memory

In summary,

+ Device controller raises an interrupt by asserting a signal on the interrupt request line
+ The CPU catches the interrupt and dispatches it to the interrupt handler.
+ The handler clears the interrupt by servicing the device

Notably, using interrupt is not the best solution

當 device 處理很快的時候 polling 會比 interrupt 效果好，因為 interrupt 會有 2 次 context switch 這個很浪費時間

**implementation**

1. How to **defer** interrupt handling during critical process? (僅延遲，不可以全部刪除)
   + CLI in x86 (clear interrupt)
   + notable, interrupt is not disappeared but just deferred.
   + However, some interrupts can not be deferred
   + most CPUs have two interrupt request line.
2. How to dispatch to the proper interrupt handler under multiple devices?
   + An interrupt vector table is used.
3. How to provide multilevel interrupts?
   + Each interrupt can be assigned an interrupt priority level
4. We need a way to get OS's attention for activities such as page faults and errors.
   + Interrupt mechanism also used for **exceptions**
   + Catch errors: page fault, dividing by zero, etc.
   + Implement system calls: by adding a special instruction, i.e., software interrupt or trap.

### FLIH and SLIH

Interrupt handling is time and resource constrained. Systems usually split interrupt management between a **first-level interrupt handler (FLIH)** and a **second-level interrupt handler (SLIH)**

FLIH: performs the context switch, state save, and queuing of a handling operation. (must be handled immediately upon an interrupt occurs)

SLIH: performs the handling of the requested operation. (can be performed later)

## Direct Memory Access

一種可以把資料從硬碟搬到記憶體的硬體

After reading data from disk to the disk controller's local buffer, the data must be moved to memory.

For large volume data transfer, most system use DMA (DMA controller transfers blocks of data from buffer storage to main memory without CPU intervention)

> **perform I/O**
>
> Use CPU to feed data into a device
>
> **DMA**
>
> Bypasses CPU to transfer data between devices and memory
>
> Requires a DMA controller

### Scatter-gather DMA

aka vectored I/O is a method of I/O by which a single procedure call sequentially reads data from multiple buffers and writes it to a single data stream, or reads data from a data stream and writes it to multiple buffers.

CPU specifies a list of sources and destination address that are not contiguous

**multiple** transfers can be executed via a **single** DMA command

### Cycle stealing from CPU

DMA controller must also use the memory bus. When DMA controller seizes the memory bus, CPU is momentarily prevented from accessing memory.

Fortunately, CPU sometimes accesses data items in its caches. 影響不大，至少比 CPU 自己搬好多了

## Application I/O Interface

OS must enable a way for users to identify a device, but within OS, UNIX identifies devices with a device identifier that is a tuple of "major and minor" device numbers.

### Block device

Commands include read(), write(), seek()

+ Accessed via file-system interface
+ Accessed via memory-mapped file

+ Accessed via raw I/O (OS 不會允許應用程式做 raw I/O)

+ Accessed via direct I/O (應用程式可以做 direct I/O, direct I/O can direct fetch File system)


+ File-based I/O
  + Buffered I/O (access data go through page)
  + Direct I/O (access data bypass the page cache)
+ Block-based I/O
  + Raw I/O

#### Memory-mapped Files

Memory mapping is convenient for programmers, usually, memory-mapped file is **efficient**

If the accessed data is not cached in memory

+ Memory-mapped files: **page fault exception** into OS
+ read/write: **system call traps** into OS

If the accessed data is cached in memory

+ Memory-mapped files: reads and writes to the file are handled as routing memory accesses **without OS** (Fast) intervention
+ read/write: still need a **system call** (Slow) into OS

Besides, memory mapped files enable easier to share file between two processes.

### Character device

include keyboard, mice, serial ports

Basic system calls, like get() or put() one character.

Libraries layered on the top of system call interface

+ Offer buffering and editing services for line editing
+ e.g., when typing a backspace, the preceding character is removed from the input stream

### Clocks and Timers

What is a clock? what is a timer?

Clock: Real-time clock (RTC)

Timer:

> Programmable interval timer (PIT)
>
> + generates a timer interrupt when it reaches a programmed amount of time
> + kernel programs PIT to issues interrupts periodically, say 1ms. This time interval is called a **tick**.

High performance event timer (高精準度計時器)

### blocking and nonblocking I/O

+ blocking: process suspended until I/O completed
+ nonblocking: I/O call returns immediately with as much as data available

+ asynchronous: return immediately, without waiting for the I/O to complete

## Kernel I/O Subsystem

+ Improve performance
  + I/O Scheduling
  + Buffering
  + Caching
  + Spooling and device reservation
+ error handling
+ protection
+ power management

### Buffering

**A memory area store data being transferred between devices.**

why we need a buffer?

1. Device speed mismatch between producer and consumer

2. Data transfer size mismatch

3. Maintain "copy semantics" for application I/O

   > problem: application calls write() to write a buffer of data to disk
   >
   > copy semantics: the data written to disk must be the version at the time of the write() system call
   >
   > solution: copy the application data into a kernel buffer in the write() system call

### Caching

**A region of fast memory holding copy data.** 

+ Always just a copy
+ Key to performance

### Spooling and device reservation

Some devices, such as printers, cannot accept interleaved data streams from multiple concurrent applications

+ sol.1 spooling
  **s**imultaneous **p**eripheral **o**peration **o**n-**l**ine 讓本來不能多工處理的裝置也能並行處理(方法是先把要印的東西寫進 memory 會 disk 中)

+ sol.2 device reservation

  Provide exclusive (獨家) access to a device. System calls for allocation and de-allocation

### Error handling

Kernel I/O subsystem must return an error number when I/O request fails. System error logs hold problem reports. For C library, an integer variable, **errno**, is used to return error code.

### I/O Protection

可以透過 user mode / kernel mode 阻擋 I/O 指令 `in` `out`

若為行動裝置，其利用 **Memory-mapped** 實現 I/O 指令則無法過濾 `in` `out` 指令，因為 memory map 操作 I/O 時可以透過 `mov` 操作，所以保護 I/O 的方法，是阻擋使用者程式去存取某部分的記憶體位置

## Transforming I/O Request to Hardware Operations

1. A process issues a blocking read() system call
   + If the data are available in page cache, the data are returned to the process, and the I/O request is completed
   + Otherwise, a physical I/O must be performed, the process if removed from the run queue and is placed on the wait queue. I/O subsystem sends the request to the device drive.

