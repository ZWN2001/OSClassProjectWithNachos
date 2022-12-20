---
title: OS课设
date: 2022-10-09 16:33:36
tags: 操作系统
category: 学习
---

# OS课设

## 环境配置

老师给了安装教程PPT，并不麻烦。

在apt install前，确认你的虚拟机网络可用。

关于虚拟机的网络配置，我参考了https://zhuanlan.zhihu.com/p/130984945，整个教程我也复制整理了一遍放在文末。

## Linux下使用CLion进行Debug

首先打开项目：将以下两个Makefile进行`Associte With File Type`，定义为GNU MakeFile文件，记得定义第一行匹配模式。完成后：

<img src="OS课设\17.png" style="zoom:60%;" />

MakeFile.common中会报错，将如下冒号替换成空格：

<img src="OS课设\18.png" style="zoom:60%;" />

然后打开具体的lab文件夹，会发现有报错，在Seetings中找到MakeFile，删掉Build target的all，选OK即可。

<img src="OS课设\19.png" style="zoom:60%;" />

对于Configurations：

<img src="OS课设\20.png" style="zoom:60%;" />

然后就可以打断点用debug模式运行了。

&nbsp;

## Lab1：Road Map Through Nachos

```text
.
├── COPYRIGHT
├── gnu-decstation-ultrix	// 交叉编译工具链
├── nachos-3.4.zip	// 未经任何修改的源码和交叉编译工具，实验就是修改源码完善各个模块的功能
├── README
└── nachos-3.4	// 实验过程中完善的代码
  ├── test	// 该目录下编写用户自己的程序，需要修改Makfile添加自己的文件
  ├── bin	// 用户自己的程序需要利用coff2noff转换，才能在nachos下跑起来
  ├── filesys	// 文件系统管理
  │   ├── directory.cc	// 目录文件，由目录项组成，目录项里记录了文件头所在扇区号
  │   ├── directory.h
  │   ├── filehdr.cc
  │   ├── filehdr.h	// 文件头数据结构，通过索引记录了文件内容实际存储的所有扇区号
  │   ├── filesys.cc	// 文件系统数据结构，创建/删除/读/写/修改/重命名/打开/关别等接口
  │   ├── filesys.h
  │   ├── fstest.cc
  │   ├── Makefile
  │   ├── openfile.cc	// 管理所有打开的文件句柄
  │   ├── openfile.h
  │   ├── synchdisk.cc	// 同步磁盘类，加锁保证互斥和文件系统的一致性
  │   ├── synchdisk.h
  │   └── test
  ├── machine	// 机器硬件模拟
  │   ├── console.cc	// 终端
  │   ├── console.h
  │   ├── disk.cc	// 磁盘
  │   ├── disk.h
  │   ├── interrupt.cc	// 中断处理器，利用FIFO维护一个中断队列
  │   ├── interrupt.h
  │   ├── timer.cc	// 模拟硬件时钟，用于时钟中断
  │   ├── timer.h
  │   ├── translate.cc	// 用户程序空间虚拟地址和物理之间的转换类
  │   └── translate.h
  ├── network	// 网络系统管理
  │   ├── Makefile
  │   ├── nettest.cc
  │   ├── post.cc
  │   ├── post.h
  │   └── README
  ├── threads	// 内核线程管理
  │   ├── list.cc	// 工具模块 定义了链表结构及其操作
  │   ├── list.h
  │   ├── main.cc	// main入口，可以传入argv参数
  │   ├── Makefile
  │   ├── scheduler.cc	// 调度器，维护一个就绪的线程队列，时间片轮转/FIFO/优先级抢占
  │   ├── scheduler.h
  │   ├── stdarg.h
  │   ├── switch.c	// 线程启动和调度模块
  │   ├── switch.h
  │   ├── switch-old.s
  │   ├── switch.s	// 线程切换
  │   ├── synch.cc	// 同步与互斥，锁/信号量/条件变量
  │   ├── synch.dis
  │   ├── synch.h
  │   ├── synchlist.cc	// 类似于一个消息队列
  │   ├── synchlist.h
  │   ├── system.cc	// 主控模块
  │   ├── system.h
  │   ├── thread.cc	// 线程数据结构
  │   ├── thread.h
  │   ├── threadtest.cc	
  │   ├── utility.cc
  │   └── utility.h
  ├── userprog	// 用户进程管理
  │   ├── addrspace.cc	// 为noff文件的代码段/数据段分配空间，虚拟地址空间
  │   ├── addrspace.h
  │   ├── bitmap.cc	// 位图，用于管理扇区的分配和物理地址的分配
  │   ├── bitmap.h
  │   ├── exception.cc	// 异常处理
  │   ├── Makefile
  │   ├── progtest.cc	// 测试nachos是否可执行用户程序
  │   └── syscall.h	// 系统调用
  └── vm	// 虚拟内存管理
  └── Makefile	// 多线程编译: make -j4
  └── Makefile.common	// 各个模块公共的Makefile内容存放到这里面
  └── Makefile.dep	// 依赖
```

<img src="OS课设\16.png" style="zoom:70%;" />

编译过程是先通过 .c 文件编译出 .o 文件，再通过 .o 文件编译出 .coff 文件，再通过 .coff 文件编译出 .noff 文件与 .flat 文件。

### Nachos Machine

> PS. 频繁出现的一个术语：例程，英文原文为routine，部分地方实际上指的是方法/函数，还有一些地方不好翻译，特此说明。

Nachos模拟了一台大致接近MIPS架构的机器。这台机器有寄存器、内存和cpu。此外，**事件驱动**的模拟时钟提供了一种机制来调度中断并在稍后的时间执行。模拟的MIPS机器可以执行任意程序。只需将指令加载到机器的内存中，初始化寄存器（包括程序计数器PCReg），然后告诉machine开始执行指令。然后机器获取PCReg指向的指令，对其进行解码并执行。该过程无限期地重复，直到执行非法操作或产生硬件中断。当陷阱或中断发生时，MIPS指令的执行被暂停，并调用Nachos中断服务例程来处理。

从概念上讲，Nachos有两种执行模式，其中一种是MIPS模拟器，类似于操作系统的用户态。Nachos通过将用户级进程加载到模拟器的内存中，初始化模拟器的寄存器，然后运行模拟器来执行用户级进程。用户程序只能访问与模拟机器相关联的内存。第二模式对应于Nachos内核。内核在Nachos首次启动时执行，或者当用户程序执行导致硬件陷阱（例如非法指令，页面故障，系统调用等）的指令时执行。在内核模式中，Nachos跟Unix进程一样执行。即执行Nachos源代码对应的语句，访问分配给Nachos变量的内存。

#### Machine Components

Nachos/MIPS机器由Machine对象实现，其中的一个实例是在Nachos首次启动时创建的。Machine对象对外提供Nachos内核直接访问的许多操作和公共变量。在下面，我们描述了Machine对象的一些重要变量。描述它们的作用有助于解释模拟硬件都做了什么。

Nachos机器对象提供寄存器、物理内存、虚拟内存支持以及运行机器或检查其当前状态的操作。当Nachos首次启动时，它会创建Machine对象的实例，并通过全局变量`machine`使其可用。Nachos内核可以访问以下公共变量：

- registers/寄存器：一共40个，具有特殊含义的寄存器在`machine.h`中有说明，如下

  ```cpp
  #define StackReg	  29	// User's stack pointer
  #define RetAddrReg	  31	// Holds return address for procedure calls
  #define NumGPRegs  	  32	// 32 general purpose registers on MIPS
  #define HiReg		  32	// Double register to hold multiply result
  #define LoReg		  33
  #define PCReg		  34	// Current program counter
  #define NextPCReg	  35	// Next program counter (for branch delay) 
  #define PrevPCReg	  36	// Previous program counter (for debugging)
  #define LoadReg		  37	// The register target of a delayed load.
  #define LoadValueReg  38	// The value to be loaded by a delayed load.
  #define BadVAddrReg	  39	// The failing virtual address on an exception
  #define NumTotalRegs  40
  //注意这里写的是寄存器下标，比如PC寄存器是第34号寄存器。
  ```

  虽然寄存器可以通过`machine->registers[x]`直接访问，但是Machine对象为此提供了`ReadRegister()`和`WriteRegister()`方法（下面将更详细地描述）。

- mainMemory/主存：内存是字节可寻址的，并组织成128字节的页，与磁盘扇区大小相同。对应于物理地址x的内存可以通过`machine->mainMemory[x]`访问。默认情况下，Nachos/MIPS机器有31页物理内存。实际使用的页数由`machine.h`中的`NumPhysPages`变量控制。
- Virtual Memory/虚拟内存：Nachos通过单个线性页表或软件管理的TLB（虽然不是同时）来提供对虚拟内存的支持。通过初始化`Machine`类的`tlb`或变量`pageTable`来进行高效控制。当执行指令时，在验证它们不是同时设置的前提下，machine对象使用定义的那个方案。

在此，我们了解了machine对象，以此来解释它如何执行任意的用户程序：首先，我们将程序的指令加载到机器的物理内存中（例如，`machine->mainMemory`变量）。接下来，我们初始化machine的页表和寄存器。最后我们调用`machine->Run()`，启动`fetch-execute`循环。

Machine对象提供以下操作/函数：

- `Machine(bool debug)`：构造函数`Machine()`接受单个参数`debug`。如果`debug`为true，MIPS模拟器会在单步模式下执行指令，在每个指令执行后调用调试器。调试器允许人们以交互方式检查机器状态，以验证（例如）寄存器或内存是否包含预期值。默认情况下，单步执行处于禁用状态。它是通过在启动Nachos时指定"-s"命令行选项来启用。
  - 对应代码`machine/machine.cc`，Line55-76。
  - 59-63初始化主存与寄存器。如果使用了TLB，65-68对TLB和页表进行初始化。（[关于TLB和分页](https://www.zwn-blog.xyz/2022/06/28/os-note7-13/#%E5%88%86%E9%A1%B5)）
  - 最后调用`CheckEndian()`检查**字节顺序**，即内存多于一个字节类型的数据在内存中的存放顺序，通常有小端、大端两种字节顺序。
  
- `RaiseException(ExceptionType which, int badVAddr)`：打印错误信息并将错误地址保存在寄存器中，结束进程中所有任务并通过中断进入内核态处理异常（或者系统调用），完成后返回用户态。

- `Debugger()`：调试器，输出调试信息，其中调用中断的`DumpState()`打印完整的中断状态，包括当前状态以及计划在未来发生的所有中断，Machine的`DumpState()`方法则打印了寄存器状态。

- `OneInstruction()`：执行指令的实际工作。它从PC寄存器获取当前指令地址，将其从内存中取出，解码，最后执行。执行方式就是`switch-case`。不过注意，**在实际访问物理内存之前，作为`fetch-execute`周期一部分引用的任何地址（包括PCReg给出的指令地址）都要通过`Translate()`方法转换为物理地址**。
  
  - `machine.h`中给出了定义但在cpp文件中并没有给出实现。
  - 具体实现在`mipssim.cc`中给出
  
- `Run()`：启动MIPS machine，启动`fetch-execute`循环。这个方法应该只有在机器寄存器和内存正确初始化后才被调用。它只是进入一个无限的`fetch-execute`循环。
  运行中的主循环做了三件事：1）它调用`Onelnstruction()`来实际执行一条指令，2）如果用户在命令
  行上请求单步模式，它调用调试器，3）它在每条指令后增加一个模拟时钟。
  
  - `machine.h`中给出了定义但在cpp文件中并没有给出实现。
  
  - 具体实现在`mipssim.cc`中给出：
  
    ```cpp
    void Machine::Run(){
        Instruction *instr = new Instruction;  // storage for decoded instruction
        if(DebugIsEnabled('m'))
            printf("Starting thread \"%s\" at time %d\n",
              currentThread->getName(), stats->totalTicks);
        interrupt->setStatus(UserMode);
        for (;;) {
            OneInstruction(instr);
       interrupt->OneTick();
       if (singleStep && (runUntilTime <= stats->totalTicks))
         Debugger();
        }
    }
    ```
  
- `int ReadRegister(int num)/void WriteRegister(int num, int value)`：不多赘述。

- `bool ReadMem(int addr, int size, int* value)`：在虚拟地址`addr`中读1、2、4字节的内存。请注意，`addr`是当前执行的用户级程序的虚拟地址；在访问物理内存之前应当调用`Translate()`。
  - 需要注意的一点是，如果地址转换失败（无论出于何种原因），ReadMem失败（返回FALSE）。因此，如果物理内存中不存在该页，则ReadMem将失败。ReadMem不会将临时故障（例如，页面不在内存中）与硬错误（例如，无效的虚拟地址）区分开来。例如，在取消引用系统调用的参数时使用ReadMem。
  
  - `machine.h`中给出了定义但在cpp文件中并没有给出实现。
  
  - `translate.cc`给出了实现
  
  - ```cpp
    bool Machine::ReadMem(int addr, int size, int *value){
        int data;
        ExceptionType exception;
        int physicalAddress;
        
        DEBUG('a', "Reading VA 0x%x, size %d\n", addr, size);
        
        exception = Translate(addr, &physicalAddress, size, FALSE);
        if (exception != NoException) {
       machine->RaiseException(exception, addr);
       return FALSE;
        }
        switch (size) {
          case 1:
       data = machine->mainMemory[physicalAddress];
       *value = data;
       break;
       
          case 2:
       data = *(unsigned short *) &machine->mainMemory[physicalAddress];
       *value = ShortToHost(data);
       break;
       
          case 4:
       data = *(unsigned int *) &machine->mainMemory[physicalAddress];
       *value = WordToHost(data);
       break;
    
          default: ASSERT(FALSE);
        }
        
        DEBUG('a', "\tvalue read = %8.8x\n", *value);
        return (TRUE);
    }
    ```
  
- `bool WriteMem(int addr,int size,int* value)`：在虚拟地址`addr`中写1、2、4字节的内存
  - `machine.h`中给出了定义但在cpp文件中并没有给出实现。
  - `translate.cc`给出了实现

&nbsp;

#### Interrupt Management

> machine/interrupt.cc & machine/interrupt.h

Nachos通过维护事件队列和模拟时钟来模拟中断。随着时钟滴答，检查事件队列以查找中计划现在发生的事件。时钟完全由软件维护，并在下列条件下滴答:

1. 每次恢复中断（并且恢复的中断掩码已启用中断）时，时钟前进一个刻度。Nachos代码经常通过显式调用`interrupt::SetLevel()`来禁用和恢复中断互斥锁。
   
   不过只有两种`Level`，开与关
   
2. 每当MIPS模拟器执行一条指令时，时钟前进一个刻度。

3. 每当准备列表为空时，时钟就会前进所需要的数量的节拍，来快进当前时间到下一个预定事件的时间。

每当时钟前进时，都会检查事件队列，并通过调用与计时器事件相关联的过程（例如中断服务例程）来服务任何挂起的中断事件。所有中断服务routine都在禁用中断的情况下运行，中断服务routine可能不会重新启用它们。

> Warning：中断处理器可能不能调用任何导致当前线程上下文切换的例程（例如，`scheduler::Run()`或`SWITCH()`）。这样做可能会导致死锁。这种限制是在 Nachos 下模拟中断方式的产物，不应该被视为在真实机器上完成事情的方式的指示。具体来说，考虑多个事件恰好在完全相同的时间被调度的情况。如果第一个事件的处理器（handler）调用sleep（调用SWITCH），则其他事件将无法在正确的时间得到服务。事实上，调用sleep的线程实际上可能正在等待现在应该发生的一个其他事件，但由于切换而延迟。因此发生死锁。
>
> 为了正确地实现抢占，当一个正在运行的线程过期时，需要切换到另一个线程，就需要调用的中断处理程序。这是通过让中断服务例程调用`Thread::YieldOnReturn()`来处理的，它会延迟实际的抢占，直到这样做是安全的。
>
> 实际上interrupt.h中也给出了这个方法，但是方法的实现有点草率
>
> > To correctly implement preemption, the interrupt handler invoked when a running threads quantum
> > expires needs to switch to another thread, This is handled by having the interrupt service routine invoke`Thread::YieldOnReturn()`,  which delays the actual preemption until it is safe to do so.

所有与中断管理相关的方法都由中断对象提供。主要包括:

- `void Schedule(VoidFunctionPtr handler, int arg, int fromnow, IntType type)`：调度未来的一个事件在时间`fromnow`发生/执行。当事件应该发生时Nachos会调用`handler`并置入参数`arg`。
  - `machine/interrupt.cc` ，生成一个`PendingInterrupt`并有序地插入事件队列。

- `IntStatus SetLevel(IntStatus now)`：将中断掩码更改为level，返回此前的level值。这个方法用于暂时禁用和重新启用中断，以达到互斥锁的目的。只支持两个中断级别: `IntOn` 和 `IntOff`，即开和关。如果正在启用中断，则通过调用 `OneTick()`使模拟的时间/时钟前进一个刻度 (advance simulated time)。

  - `machine/interrupt.cc` ，Line 115-126

  > - This routine is used to temporarily disable and reenable interruptsfor mutual exclusion purposes.Only two interrupt levels are supported: IntOn and IntOff 

- `OneTick()`：将时钟提前一个刻度，并为任何待处理的请求提供服务（通过调用`CheckIfDue()`）。它在每个用户级指令执行后在`machine::Run()`调用，以及在中断恢复时由`SetLevel()`调用。

  - `machine/interrupt.cc` ，Line 150-177

  - ```cpp
    void Interrupt::OneTick(){
        MachineStatus old = status;
    
    // advance simulated time
        if (status == SystemMode) {///内核态
            stats->totalTicks += SystemTick;
       stats->systemTicks += SystemTick;
        } else {               // 用户态
       stats->totalTicks += UserTick;
       stats->userTicks += UserTick;
        }
        DEBUG('i', "\n== Tick %d ==\n", stats->totalTicks);
    
    // check any pending interrupts are now ready to fire
    ///为任何待处理的请求提供服务
        ChangeLevel(IntOn, IntOff);       // 关中断
                   // (interrupt handlers run with interrupts disabled)
        while (CheckIfDue(FALSE));    // check for pending interrupts
    
        ChangeLevel(IntOff, IntOn);       // 开中断
        if (yieldOnReturn) {      // if the timer device handler asked for a context switch, ok to do it now （可能的上下文切换）
       yieldOnReturn = FALSE;
       status = SystemMode;      // yield is a kernel routine
       currentThread->Yield();   //内核态下调用Yield()
       status = old;
        }
    }
    ```

- `bool CheckIfDue(bool advanceClock)`：检查事件队列中现在需要服务的事件。如果它找到了，它就为他们服务。它在诸如 `OneTick()` 之类的地方被调用。若`advanceClock`为true则前进时钟。
  
- `Idle()`：将时钟前进到下一个需要调度的事件的时间点。当没有线程在就绪队列并且我们希望加速时钟前进时会被调度器调用（有点怪，但原文如此。）。因为必须运行某些东西才能将线程放入就绪队列，所以我们要做的唯一一件事就是提前模拟时间，直到下一个预定的硬件中断。

  - 简单来说，函数会进入`IdleMode`，一直进行时钟的前进直到找到需要调度的中断，并在此时间点检索其余的所有中断，如果找到了，将状态设置为`SystemMode`并返回，此时我们就有了一个需要执行的线程，因此进程不应当被终结。

  - 当没有中断需要被调度也没有就绪态的任务时调用`Halt()`结束进程。

- 其余的函数都比较简单。

&nbsp;

#### Real-Time Clock Interrupts

> machine/timer.cc & machine/timer.h
>

Nachos提供了一个模拟实时时钟的Timer对象，以固定的时间间隔生成中断。它使用上述相同的事件驱动中
断机制来实现。Timer支持以下操作：

- `Timer(VoidFunctionPtr timerHandler, _int callArg, bool doRandom)`：Timer构造方法创建了每`TimerTicks`（100）个时间单元（时钟周期）的实时时钟。当当前时间片用完，Nachos调用`timerHandler`，参数为`callArg`。若`doRandom`为true，时间片的取值会是从一到两倍`TimerTicks`的随机数。最后调用`interrupt->Schedule`开始调度。
- `TimeOfNextInterrupt() `：获取下次中断的随机数。

> PS. 使用"-rs"选项启动Nachos会创建一个定时器对象，该对象以随机间隔中断并抢占当前运行的线程。
>
> 这些代码并不是Nachos的一部分，这只是对 Nachos 运行的硬件的模拟。
>
> DO NOT CHANGE -- part of the machine emulation

&nbsp;

#### Address Translation

> machine/translation.cc & machine/translate.h

Nachos支持两种类型的VM体系结构：线性页表或软件管理的TLB。前者更容易编程，后者更接近于当前`machine`支持的内容。Nachos 支持一个或另一个，但不是两个（同时）。

#### 线性页表

对于线性页表，MMU将虚拟地址拆分为页码和页偏移分量。页码用于索引到页表条目的数组中。实际物理地址是页表项中的页帧号与虚拟地址的页偏移量的串联。

要使用线性页表，只需初始化变量`machine->pageTable`以指向用于执行翻译的页表。一般来说，每个用户进程都会有自己的私有页表。因此，进程切换需要更新`pageTable`变量。在真实的机器中，`pageTable`将对应于一个特殊的寄存器，该寄存器将作为`SWITCH()`操作的一部分保存和恢复。机器变量`pageTableSize`表示页表的实际大小。

页表条目由相应虚拟页的物理页帧号、指示该条目当前是否有效的标志(由OS设置，由硬件检查)、指示该页是否可能被写入的标志(由OS设置，由硬件检查)、指示该页是否已被引用的位(由硬件设置，由OS检查和清除)和脏位(由硬件设置，由OS检查和清除)组成。Nachos机器具有从主内存开始的物理内存的数字页。因此，页第0页从`machine->mainMemory`开始，而页第N页从`machine->mainMemory + N · PageSize`开始。

关键代码就在208、209、251行。

此外，`translate.cc`还定义了物理地址与虚拟地址（实际上是虚拟页号与物理帧的对应关系）的实体类：

```cpp
class TranslationEntry {
  public:
    int virtualPage; // The page number in virtual memory.
    int physicalPage; // The page number in real memory (relative to the start of "mainMemory"
    bool valid;  // If this bit is set, the translation is ignored.(In other words, the entry hasn't been initialized.)
    bool readOnly; // If this bit is set, the user program is not allowed to modify the contents of the page.
    bool use; // This bit is set by the hardware every time the page is referenced or modified.
    bool dirty; // This bit is set by the hardware every time the page is modified.
};
```

&nbsp;

#### Console Device

> machine/console.cc & machine/console.h

- `Console(char *readFile, char *writeFile, VoidFunctionPtr readAvail, VoidFunctionPtr writeDone, _int callArg)`：构造函数。参数`readFile`包含要从中读取数据的Unix文件名；如果为NULL，则假定为标准输入。同样，参数`writeFile`指示写入控制台的输出要去哪里；如果为NULL，则假定标准输出。当一个字符可供读取时（比如敲键盘），调用以`callArg`作为参数的`readAvail`这个中断控制器来通知Nachos一个字符可用。通过调用`Console::GetChar()`检索字符本身。返回时，假设该字符已被检索，当下一个到达时，将再次调用`readAvail`。当一个字符被输出时调用`writeDone`中断控制器以使得下一个下一个字符可以请求输出。

- `void PutChar(char ch)`：将字符`ch`写入输出设备。一旦输出启动，在相应的IO结束中断发生之前再次调用`PutChar()`将会抛出Error。一旦控制台设备将字符写入设备，它就会调用用户提供的`writeDone`，并将`callArg`作为参数传递给他。

- `char GetChar()`：从控制台检索字符。如果没有新的输入数据，GetChar返回EOF。通常，用户不会调用 GetChar，除非新数据的可用性已经通过 readAvail 中断服务例程发出信号（unless the availability
  of new data had first been signalled via the readAvail() interrupt service routine）
- `void CheckCharAvail()`：检查是否有数据可以读。最初会调度下一次读输入，完成读入后调用`readHandler`处理（53行`readHandler = readAvail;`）
- `WriteDone()`：当需要调用 interrupt handler 来告诉 Nachos 内核输出字符已经完成时，内部例程会调用这个方法。方法中只是设置了忙位为空闲，更新输出字符的长度累计，最后调用`writeHandler`进行处理。

当构造函数创建 console 时，将打开适当的Unix文件（或stdinstdout），且 timer event 会被安排在未来某个时间点占用100个时间单元。当 timer 到期时，会调用例程`CheckCharAvail`以查看是否存在数据。如果存在，`CheckCharAvail`会读取该字符并调用用户提供的输入中断处理程序 `readAvail`。然后，它会计划一个新的 timer event，以便该过程每100个时间单位重复一次。因此，`CheckCharAvail`只需每100个时钟周期轮询一次新数据，并在数据存在时调用中断服务例程进行处理。

设备输出是通过调用PutChar初始 化的，给它一个字符来输出。一旦字符输出被初始化，设备将处于忙碌状态，直到输出完成中断（output complete interrupt）发生。PutChar只输出一个字符，设置一个内部标志以指示设备忙，然后安排一个定时器中断在100个时钟滴答之后发生。当定时器超时时，设备的状态从忙变为空闲，并调用用户提供的输出中断完成例程。如果其他输出字符排队等待输出，则此例程可能会调用PutChar。

&nbsp;

#### Disk Device

> machine/disk.cc & machine/disk.h

Disk对象模拟真实磁盘的行为，只有一个磁盘，多个轨道以及独立的扇区。每个轨道包含相同数量的扇区，并且块由它们的扇区号唯一标识。与真实磁盘一样，操作系统启动操作以读取或写入特定扇区，并且稍后的中断指示操作何时实际完成。Nachos磁盘在同一时间点只允许一个挂起的操作；只有当设备空闲时，操作系统才可以启动新的操作。请注意，操作系统有责任确保在磁盘忙于处理早期请求时不会发出新请求。

为了模拟访问磁盘的典型延迟，Nachos磁盘对象动态地改变IO操作的启动与其相应的IO完成中断之间的时间。实际延迟取决于将磁盘头从其先前位置移动到新轨道需要多长时间，以及在读写头下旋转所遇到的旋转延迟。

模拟磁盘包含`NumTracks(32)`磁道，每个磁道包含`Sectorspertrack(32)`扇区。每个扇区的大小为`SectorSize` (128)字节。此外，磁盘包含"轨道缓冲区"缓存。在寻找一个新的轨道后，磁盘立即开始读取扇区，将它们放在轨道缓冲区中。这样，后续的读取请求可能会找到缓存中已有的数据，从而减少访问延迟。

Disk对象有以下函数：

- `Disk(const char* name, VoidFunctionPtr callWhenDone, _int callArg)`：此构造函数假定模拟磁盘保存在名为`name`的Unix文件中。如果该文件还不存在，Nachos将创建它并将0x456789ab的"magic number"写入最初的四个字节。magic number的存在允许Nachos将包含nachos模拟磁盘的文件与包含其他内容的文件区分开来。最后，Nachos确保文件的其余部分包含空扇区。所有的Nachos磁盘具有相同的大小，即 $NumSectors\times SectorPerTrack+sizeof(int)$ ,`sizeof(int)`是指magic number所占的空间。（也可能指最后写入tmp的大小）。

  `Lseek()`函数会重新定位被打开文件的位移量，最后将`tmp`写到文件结尾，这样不会返回EOF。

  如果文件已经存在，Nachos读取前4个字节以验证它们是否包含预期的Nachos "magic number"，如果检查失败则终止。请注意，默认情况下，Nachos 磁盘的内容是跨多个 Nachos 会话保存的，也就是说，允许用户在一个会话中创建一个 Nachos 文件，并在另一个会话中读取它。但是，如果磁盘包含文件系统，并且文件系统被先前的Nachos会话保留在损坏状态，那么如果不首先验证文件系统数据结构在逻辑上是否一致，则后续的Nachos调用可能会遇到问题。

  最后两个构造函数参数用于提供IO完成中断机制。具体来说，Nachos机器通过调用方法`callWhenDone`来发出磁盘操作（例如读或写）完成的信号，并传递`callArg`的参数。后文会提到，`SynchDisk`对象使用此例程唤醒在等待IO完成时已挂起的线程。

- `PrintSector(bool writing, int sector, char *data)`，在读就不会打印，否则将sector数据打印。

- `ReadRequest(int sectorNumber, char* data)`：被调用来读取指定的扇区号数据放入缓冲区数据中。我们使用`SectorSize * sectorNumber + MagicSize`计算偏移量，使用`Read(fileno, data, SectorSize);`进行读取。

  `ReadRequest`安排一个中断在未来的某个时间（ticks）发生，大约取决于完成操作所需的寻道距离。代码118行以及代码132行的`interrupt->Schedule(DiskDone, (_int) this, ticks, DiskInt);`也指明了这一点，这个中断标明读取完成。

- `WriteRequest(int sectorNumber, char* data)`：跟读类似。

- `TimeToSeek(int newSector, int *rotation) `：计算磁头移动到正确位置所需时间。`stats.h`中定义了`SeekTime`为500，以此为基准量进行模拟计算。
- `ComputeLatency`：返回从磁头的当前位置读/写一个磁盘扇区需要多长时间。`Latency = seek time + rotational latency + transfer time`



#### Stats

头文件中定义了以下常数，用于反映一个操作在实际系统中所花费的相对时间。一个“刻度”就是一个时间单位——比如，一微秒。

由于 Nachos 内核代码是直接执行的，并且在内核中花费的时间是通过调用中断的次数来衡量的，因此这些时间常数并不太精确。

```cpp
#define UserTick 	1	// advance for each user-level instruction 
#define SystemTick 	10 	// advance each time interrupts are enabled
#define RotationTime 	500 	// time disk takes to rotate one sector
#define SeekTime 	500    	// time disk takes to seek past one track
#define ConsoleTime 	100	// time to read or write one character
#define NetworkTime 	100   	// time to send or receive one packet
#define TimerTicks 	100    	// (average) time between timer interrupts
```

其余只是用于统计性能数据。

其余mipssim用于适配 Ousterhout's MIPSSIM package，network不考虑，sysdep，如其名，系统依赖。

&nbsp;

### Nachos Threads

 在Nachos与其他很多操作系统中，一个进程主要包括以下几部分：

1. 一个地址空间。地址空间包括进程被允许引用的所有内存。在某些系统中，两个或多个进程可能共享一个地址空间的一部分，但在传统系统中，一个地址空间的内容对该进程是私有的。地址空间进一步分解为 1) 可执行代码(例如程序的指令)、2) 局部变量的堆栈空间和 3) 全局变量和动态分配内存的堆空间(例如由Unix malloc或c++ new运算符获得)。在Unix中，堆空间进一步分解为BSS（包含初始化为0的变量）和数据部分（初始化的变量和其他常量）。

2. 一个控制线程。
3. 其他对象，如打开文件描述符。

也就是说，一个进程由一个程序（program）、它的数据（data）和所有相关联的状态信息（state information）（存储器、寄存器、程序计数器、打开的文件等）组成。

有时允许多个控制线程在单个进程中并发执行能够提高系统效率。这些单独的控制线程称为 `threads`。默认情况下，进程只有一个线程与它们关联，尽管关联多个线程可能很有用。**特定进程的所有线程共享相同的地址空间。相反，我们通常认为进程不与其他进程共享它们的任何地址空间**。具体来说，线程具有与其相关联的代码，内存和其他资源。虽然线程与该进程的其他线程共享许多对象，但线程有自己的私有本地堆栈。

> 实际上，线程在技术上确实与其他线程共享其堆栈，因为特定线程的堆栈仍可由其他线程寻址。但是，一般来说，将堆栈视为私有堆栈更有用，因为每个线程都必须有自己的堆栈。
>
> Actually，threads technically do share their stacks with other threads in the sense that a particular
> thread‘s stack will still be addressable by the other threads。 In general， however， it is more useful to think of the stack as private， since each thread must have its own stack.
>
> (有点绕）

**线程和进程之间的一个很大的区别是全局变量在所有线程之间共享**。由于线程与其他线程并发执行，因此它们在访问共享内存时必须考虑同步和互斥。Nachos提供线程。Nachos线程执行并共享相同的代码（Nachos源代码）并共享相同的全局变量。Nachos调度程序维护一个称为就绪列表 `ready list` 的数据结构，它跟踪准备执行的线程。Ready列表上的线程已准备好执行并且可以随时选择由调度器执行。每个线程都有一个相关联的状态，描述线程当前正在做什么。Nachos的线程处于四种状态之一：

- 就绪/READY：线程有资格使用CPU（例如，它在就绪列表中），但另一个线程恰好正在运行。当调度程序选择要执行的线程时，它会将其从就绪列表中删除，并将其状态从就绪更改为正在运行。只有处于就绪状态的线程才能在就绪列表中找到。

- 运行/RUNNING：线程当前正在运行。某一时间点只有一个线程可以在运行中。在Nachos中，全局变量`currentThread` 始终指向当前运行的线程。

- 阻塞/BLOCKED：线程因等待一些外部事件而被被阻塞，直到事件发生才有可能继续执行。具体来说，线程已经通过 `Thread::Sleep()` 将自己置于睡眠状态。它可能正在等待条件变量，信号量等。根据定义，阻塞的线程不驻留在就绪列表上。
- 创建/JUST_CREATED：线程存在，但还没有分配堆栈。这种状态是暂时的线程创建期间使用的状态。Thread构造函数可以创建一个线程，而 `Thread::Fork()` 实际上将线程转换为CPU可以执行的线程（例如，将其放在就绪列表中）。

在`thread/thread.h` Line 61 中我们也可以找到 `enum ThreadStatus { JUST_CREATED, RUNNING, READY, BLOCKED };`。

在非面向对象的系统中，操作系统维护称为进程表（process table）的数据结构。进程表项包含与进程相关联的所有信息（如保存的寄存器内容、当前状态等）。过程表信息通常称为上下文块（`context block`）。与其他系统相比，Nachos不维护明确的过程表。相反，与线程相关联的信息被保持为（通常）`Thread` 对象实例的私有数据。

传统操作系统将线程信息集中在单个表中，而Nachos 中没有显式定义 PCB，而是将进程信息分散到相应的类对象中；例如利用所分配内存空间的对象指针表示一个进程，该对象中含有进程的页表、栈指针以及与核心线程的映射等信息。进程的上下文保存在核心线程中，当一个线程被调度执行后，依据线程所保存的进程上下文中执行所对应的用户进程，仍然有较大的改进空间。

Nachos的 `Thread` 对象支持以下操作：

> thread/thread.cc & thread/thread.h

- `Thread(const char* threadName)`：构造函数，只进行了一些初始化，在此之后我们就可以进行`Fork()`

- `Fork(VoidFunctionPtr func, _int arg)`：参数func是当线程开始执行时开始执行的任务的地址（address of a procedure）。参数arg是一个应该传递给新线程的参数。函数会分配一个堆栈并初始化堆栈与寄存器，使其在被 `SWITCH` 时能运行，然后在关中断的条件下将线程放在就绪队列中。

  我们必须考虑一个重要细节即：当任务/函数 func返回时应该发生什么？由于func不是作为常规过程调用调用的，因此它无法return，或者说，no place for it to return to。实际上，运行func的线程应该终止，而不是返回。Fork通过构建一个初始激活记录（initial activation record）来处理这个细节（下面将详细描述）。

- `StackAllocate (VoidFunctionPtr func, _int arg)`：函数完成分配堆栈和创建初始激活记录的工作，初始激活记录会导致执行看起来是在 func 中开始的。具体执行如下工作：

  - 为堆栈分配内存。默认堆栈大小为$StackSize(4096) \times 4字节$。即：

    ```cpp
    #define StackSize	(sizeof(_int) * 1024)
    stack = (int *) AllocBoundedArray(StackSize * sizeof(_int));
    ```

    当然，这个大小可能不够。

  - 在分配的堆栈的顶部放置一个sentinel值。每当它切换到新线程时，调度程序都会验证正在切换出的线程的sentinel值是否未更改，如果线程在执行期间溢出其堆栈，则可能会发生这种情况。
    
  - 初始化程序计数器PC以指向例程ThreadRoot。而不是从用户提供的例程开始执行。ThreadRoot会调用初始化例程（函数）以启用中断，然后调用用户提供的函数，并将提供的参数传递给它。最后调用`thread::Finish()`，终止线程。

    让线程执行在ThreadRoot中开始，而不是在用户提供的例程中开始，这使得在线程完成工作后终止线程变得简单。ThreadRoot的代码是用汇编语言编写的，可以在switch中找到。（`switch.s `Line48）

    注意：ThreadRoot不是调用Fork()后创建的新线程运行的。新创建的线程在调度并开始执行时会执行ThreadRoot中的指令。

- `Yield ()`：挂起调用线程并选择一个新的执行（通过调用`Shheduler::FindNextToRun()`)，并将线程放在就绪列表的末尾，这样它最终将被重新调度。如果没有其他线程准备执行，则将函数返回以使其继续在当前线程中运行。或者当线程最终工作到就绪列表的最前面并得到重新调度时会直接返回。注意: 开始时我们禁用了中断，这样就可以自动查看就绪列表最前面的线程并切换到它（保证一致性）。在返回时，我们将中断级别重置为其原始状态，以防在调用时中断被禁用。与 `Thread::Sleep ()`类似，但不同。

- `Sleep()`：挂起当前线程，将其状态更改为BLOCKED，并将其从就绪列表（Ready List）删除。如果就绪列表为空，则调用`interrupt->Idle()`等待下一个中断。当当前线程需要被阻塞，直到某个未来事件发生时调用Sleep。这个"未来事件"的责任是唤醒阻塞的线程并将其移回就绪列表。我们关闭中断，这样在从就绪列表中获取第一个线程并切换到它时就不会有时间切片（time slice）。

  > We need interrupts off so that there can't be a time slice between pulling the first thread off the ready list, and switching to it.

- `Finish()`：当线程执行fork完成时，由 ThreadRoot 调用。

  注意: 我们不会立即取消分配线程数据结构或执行堆栈，因为我们的线程仍然运行在堆栈上。相反，我们设置 `threadToBeDestroied`变量为将被终止的线程，这样一旦我们在另一个线程的上下文中运行，新线程的`Scheduler: : Run()`将调用被终止线程的析构函数，也就是说，新调度的线程将检`threadToBeDestroyed`变量并完成终止任务。注意: 我们禁用了中断，这样我们就不会在设置 `threadToBeDestroied` 和调用`Sleep()`之间获得时间片。

- `CheckOverflow()`：这个后文会提到

- `SaveUserState()`&`RestoreUserState()`：保存/恢复寄存器。

&nbsp;

#### Mechanics of Thread Switching

> 汇编写的，图一乐，我也看不怎么懂：thread/switch.s & thread/switch.h

将CPU从一个线程切换到另一个线程涉及挂起当前线程，保存其状态（例如，寄存器），然后恢复正在切换到的线程的状态。线程切换实际上在一个新的程序计数器加载到PC的那一刻完成。在此之后，CPU不再执行线程切换代码而是执行与新线程相关的代码。

Switch执行以下操作:

1. 将所有寄存器保存在`oldThread`的上下文块中。

2. 我们希望在调用`Switch()`之后立即在指令处恢复执行，因此，我们不保存当前PC，而是将返回地址（在线程的激活记录（tivation record）中找到）放在线程的上下文块（context block）中。当稍后恢复线程时，加载到PC中的恢复地址将是紧接在前面调用`Switch()`之后的指令。
   注意：对于调用它者来说，`Switch()`似乎是一个常规的过程调用是至关重要的。这样，线程可以随时调用`Switch()`。该调用将看起来像正常过程调用一样返回，除了返回不会立即发生。只有在调度程序决定恢复切换线程的执行之后，它才会再次运行。

3. 保存当前线程的状态后，从下一个线程的上下文块恢复寄存器的值。

上下文切换发生在什么时候？当前PC被进程表中找到的保存的PC替换时。一旦保存的PC被加载，`Switch()`不再执行。我们现在正在执行与新线程相关联的指令，该指令应该是在调用`Switch()`之后立即执行的指令。新的PC被装载，也就相当于发生了上下文切换。

&nbsp;

#### Threads & Scheduling

> thread/schedule.cc & thread/schedule.h

准备运行的线程将保留在就绪列表中。只有当进程具有所需的所有资源（CPU除外）时，进程才处于就绪状态。进程阻塞等待IO，内存等。通常存储在与正在等待的资源相关联的队列中，这与进程的就绪队列类似。

调度程序决定接下来运行哪个线程。每当当前线程希望放弃CPU时，都会调用调度程序。例如，当前线程可能已经启动了IO操作，并且必须等待它完成才能进一步执行。或者，Nachos可能会抢占当前线程，以防止一个线程垄断CPU。

Nachos调度策略很简单：线程驻留在单个无优先级顺序的就绪列表（Ready list）上，线程以循环方式选择（或者说就是队列）。也就是说，线程总是附加到就绪列表的末尾，并且调度程序总是选择列表的第一个线程。

调度由Scheduler对象中的方法处理（此Schedule非彼Schedule）：

- `Scheduler()`：构造方法，初始化就序列表。
- `ReadyToRun (Thread *thread)`：将线程`thread`状态设为READY并附加到就序列表末尾。
- `FindNextToRun()`：返回列表第一个线程，没有就返回NULL。
- `Run (Thread *nextThread)`：挂起当前线程并运行新的线程。注意是当前正在运行的线程去调用`Run()`。函数执行以下任务：
  - 首先检查是否溢出堆栈，调用thread 的`CheckOverflow()`，至于溢出标志位上文提到过。如果正在运行的线程溢出其堆栈，则栈顶的sentinel值将被复盖，从而更改其值。通过在每次切换线程时检查sentinel值，我们可以捕获溢出堆栈的线程。 
  - 将新选择的线程的状态更改为正在运行。Nachos假设当前线程调用`Run()`前已经将其状态更改为其他状态如READY，BLOCKED
    - Line 106
  - 实际上通过调用`Switch()`切换到下一个线程。Switch返回后，我们现在作为新线程执行。但是请注意，由于线程从`Run()`切换到以前调用的Switch，因此在调用Switch后立即在`Run()`语句处继续执行。
    - Line 116
  - 如果前一个线程正在终止自身（如threadToBeDestroyed变量所示），立即将其终止（在`Switch()`之后）。线程不能直接连接自己；另一个线程必须这样做。重要的是要理解它实际上是另一个线程物理终止调用`Finish()`的线程。
    - Line 124 -127

#### Synchronization and Mutual Exclusion

> thread/synch.cc & thread/synch.h

[经典信号量机制](https://www.zwn-blog.xyz/2022/06/28/os-note1-6/#%E4%BF%A1%E5%8F%B7%E9%87%8F%EF%BC%88-semaphore%EF%BC%89)。不多解释。

&nbsp;

#### Special Notes

当Nachos第一次开始执行时，它作为单个Unix进程执行。Nachos将此单个用户进程转换为单个nachos线程。因此，默认情况下，Nachos执行单个线程。当nachos入口点方法 main 返回时，该线程也会退出。但是，如果其他线程已创建并继续存在，则Unix进程继续执行Nachos。只有在所有线程终止后，Unix Nachos进程才会退出。

&nbsp;

### User-Level Processes

Nachos在自己的私有地址空间中运行用户程序。Nachos可以运行任何MIPS二进制文件，假设它将自己限制为仅进行Nachos能理解的系统调用。在Unix中，"a.out"文件以"coff"格式存储。Nachos要求可执行文件采用更简单的"Noff"格式。要将一种格式的二进制文件转换为另一种格式，请使用coff2noff程序。有关详细信息，请参阅test目录中的Makefile，或者可以直接在bin目录下找到`coff2noff.c`。

Noff格式文件由四个部分组成。第一部分是Noff头，描述了文件其余部分的内容，给出了有关程序指令，初始化变量和未初始化变量的信息。Noff头位于文件的开头，并包含指向其余部分的指针。具体来说，Noff头包含：

```cpp
//noff.h

typedef struct segment {
  int virtualAddr;		/* location of segment in virt addr space */
  int inFileAddr;		/* location of segment in this file */
  int size;			/* size of segment */
} Segment;

typedef struct noffHeader {
   int noffMagic;     /* should be NOFFMAGIC */
   Segment code;      /* executable code segment */ 
   Segment initData;      /* initialized data segment */
   Segment uninitData;    /* uninitialized data segment --
             * should be zero'ed before use 
             */
} NoffHeader;
```

- `noffmagic`：一个保留的magic number，表明该文件是在Noff格式。magic number存储在文件的前四个字节中。在尝试执行用户程序之前，Nachos检查魔术数字以确保即将执行的文件实际上是nachos可执行文件。
  - 对于剩余的每个部分，Nachos维护以下信息：
    - `virtualAddr`： 该段开始的虚拟地址(通常为零)。What virtual address that segment begins at （normally zero）
    - `inFileAddr`： Noff 文件中的指针，该段实际开始的地方(这样 Nachos 可以在执行开始之前将其读入内存)。Pointer within the Noff file where that section actually begins （so that Nachos can read it into memory before execution begins）
    - `size`：该段的大小（以字节为单位）

**当执行程序时，Nachos创建一个地址空间，并将指令的内容和初始化的变量段复制到地址空间中**。请注意，**不需要从文件中读取未初始化的变量部分**。由于它被定义为全零，Nachos只是在Nachos进程的地址空间内为其分配内存并将其清零。

#### Process Creation

Nachos进程是通过创建地址空间、为地址空间分配物理内存、将可执行文件的内容加载到物理内存、初始化寄存器和地址转换表、然后调用`machine::Run()`来启动执行。`Run()`只是启动模拟MIPS机器，让它进入一个无限循环，一次执行一个指令。

Nachos假设在给定时间只有一个用户程序存在。因此，当创建地址空间时，Nachos假定没有其他人正在使用物理内存，并且只是将所有物理内存（例如mainMemory字符数组）归零。然后，Nachos从位置mainMemory开始将二进制文件读取到物理内存中，并初始化转换表以在虚拟地址和物理地址之 间进行一对一映射（例如，使任何虚拟地址N直接映射到物理地址N）。寄存器的初始化包括将它们全部归零，分别将`PCReg`和`NextPCReg`设置为0和4，并将`stackpointer`设置为进程的最大虚拟地址（the stack grows downward towards the heap and text）。Nachos假设用户程序的执行始于文本段中的第一条指令（例如，虚拟地址0）。

当添加了对多个用户进程的支持时，进程切换需要另外两个Nachos例程。每当当前进程被挂起（例如，抢占或进入睡眠状态）时，调度程序都会调用例程`AddrSpace::SaveUserState()`，以便正确保存低级线程切换例程不知道的地址空间相关状态（in order to properly save address space related state that the low level thread switching routines do not know about），这在使用虚拟内存时变得必要。当从一个进程切换到另一个进程时，需要加载一组新的地址转换表。每当nachos调度程序即将抢占一个线程并切换到另一个线程时，Nachos调度程序都会调用`SaveUserState()`。同样，在切换到新线程之前，Nachos调度程序调用`AddrSpace::RestoreUserState`。`RestoreUserState()`能确保在执行恢复之前加载正确的地址转换表。

#### Creating a Noff Binary

Nachos能够执行包含任意MIPS指令的程序。例如，在MIPS机器上使用gcc编译test目录中的C程序以创建".o"的文件。要创建一个a.out二进制文件，加载程序在用户程序代码之前预置 `test/start.s` 中的指令。`start.s`包含需要在用户主程序之前执行的初始化代码。具体来说，`start.s`最开始的指令调用用户提供的`main`函数，而第二条指令调用Nachos `Exit`系统调用，以确保用户进程在主程序return时正确终止。另外，`start.s`包含用于调用系统调用的`stub modules`（如下所述）。

#### System Calls and Exception Handling

用户程序通过执行MIPS的 `syscall`指令来调用系统调用，该指令通常会通过硬件陷阱（hardware trap）来陷入到Nachos内核中。Nachos MIPS模拟器通过调用例程`RaiseException()`来实现trap，并向其传递指示trap的确切原因的参数。反过来，`RatseException`调用`ExceptionHandler`来处理特定的问题。`ExceptionHandler`接收一个参数，指示trap的确切原因。

`Syscall`指令指示请求系统调用，但不指示执行哪个系统调用。按照惯例，用户程序在执行`syscall`指令之前将指示所需的特定系统调用的代码放在寄存器r2中。系统调用的其他参数（适当时）可以在寄存器r4-r7中找到，遵循标准的C过程调用链接约定。函数（和系统调用）返回值预计在返回时存入寄存器r2中。

> Warning：从异常处理程序（或一般在Nachos中）访问用户内存时，不能直接引用用户级地址。用户级进程在自己的私有地址空间中执行，内核无法直接引用这些地址空间。尝试取消引用指针作为系统调用传递的参数，如果直接引用，可能会导致问题（例如分段错误）（Attempts to dereference pointers passed as arguments to system calls will likely lead to problems (eg segmentation faults) if referenced directly）。使用`ReadMem` 和 `WriteMem`来做到这一点。
>

#### Execution Trace of User - Level Process

考虑最简单的Nachos程序`halt.c`，它调用`Halt`系统调用，不做任何其他事情。

编译时，`gcc -S`生成如图所示的汇编语言代码（为了清楚起见，添加了行号和附加注释）。与调用`halt`系统调用直接相关的唯一指令在第27行中给出。`jal`指令执行将控制转移到标签`Halt`的跳转。请注意，此程序中没有称为`Halt`的标签。`Halt`的代码可以在`start.s`中找到。第1-21行是汇编器指令，实际上不会生成任何指令。第22-25行执行标准过程调用链接操作，该操作是作为在新子例程中最开始执行时的部分执行的（Lines 22-25 perform the standard procedure call linkage operations that are performed as part of starting execution in a new subroutine）（例如，保存返回地址并在堆栈上为局部变量分配空间）。第26行是对gcc提供的库例程的调用，Unix需要在程序正常开始执行之前执行该例程（在Nachos中不需要它，因此提供了一个不执行任何操作的虚拟例程）。第29-33行提供了从过程调用返回的标准代码，有效地撤消了第22-25行的影响（例如，获取保存的返回地址，然后跳回它）。

<img src="OS课设\12.png" style="zoom:80%;" />

在`start.s`中我们能找到进行系统调用的实际说明。图3显示了halt程序使用的该文件的子集。同样，第1-7行是汇编器指令而不是指令。第8行是第一个实际指令，只是调用了`main`，例如`halt.c`中的主程序`main`。每当对`main`的调用返回时，都会执行第9-10行，在这种情况下，我们通过`Exit`系统调用想告诉Nachos我们已经完成了相应任务。

<img src="OS课设\13.png" style="zoom:80%;" />

我们将系统调用的代码放入二号寄存器中，然后执行系统调用指令，就可以在运行时调用系统调用。`Syscall`指令是一个陷阱指令，意味着下一个要执行的指令将是陷阱处理程序的第一条指令。在Nachos中，这实际上意味着执行现在继续在过程`ExceptionHandler`中进行。考虑第23-27行，第24行在寄存器r2中放置"Exit"的编码，第25行执行实际的系统调用。第26行，将在系统调用后执行的第一条指令简单地返回给调用者。请注意，退出系统调用通常不会返回（如果编码正确），但无论如何都提供了return。

图四说明了从`halt.c`源代码转换到可执行程序的过程。第1-3行从`halt.c`生成目标代码与`start.s`源文件。第5行创建可执行二进制文件。注意第五行中注我们在 `halt.o` 之前列出 `start.o`， 这样可以确保 `start.s` 中的代码位于主程序之前（Note that listing start.o before halt.o insures that the code in start.s resides before that of the main program）。最后，第6行将可执行文件转换为Noff格式，使其准备在Nachos下执行。

<img src="OS课设\14.png" style="zoom:80%;" />

实用程序`disassemble`（`bin/sisasm.c`）可用于显示halt二进制文件中的实际指令。反编译`halt.noff` 的结果如图五所示，这些指令跟上面描述的相同，不过编码更难理解，因为标签已被替换为地址。

<img src="OS课设\15.png" style="zoom:80%;" />

#### 地址空间

```cpp
class AddrSpace {
  public:
    ///创建并初始化地址空间，并存出现在打开的文件中
    AddrSpace(OpenFile *executable);   
    ~AddrSpace();        // De-allocate an address space

    ///在进入用户代码前初始化用户级CPU 寄存器
    void InitRegisters();     

    ///存或者重存特定地址空间
    ///在上下文切换时保存机器状态
    void SaveState();        // Save/restore address space-specific
    ///上下文切换后恢复状态
    void RestoreState();      // info on a context switch 

  private:
    ///页表，线性数组
    TranslationEntry *pageTable; 
    ///虚拟地址空间的页数
    unsigned int numPages;   
```

- 构造方法

```cpp
///创建地址空间以运行用户程序。
///从一个可执行文件中加载程序，并进行设置，以便我们可以开始执行用户指令。
///假定目标代码文件为 NOFF 格式。
///首先，设置从程序存储器到物理存储器的转换。现在，这真的很简单(1:1) ，
/// 因为我们只是单一编程，我们有一个单一的未分段的页表。
///“可执行文件”是包含要加载到内存中的目标代码的文件
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable){
    NoffHe ader noffH;
    unsigned int i, size;

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
      (WordToHost(noffH.noffMagic) == NOFFMAGIC))
       SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
         + UserStackSize;   // we need to increase the size
                  // to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

    ASSERT(numPages <= NumPhysPages);     // check we're not trying
                  // to run anything too big --
                  // at least until we have
                  // virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
               numPages, size);
// first, set up the translation
///当构造一个地址空间时，将设置一个页表，并将整个机器内存归零
    pageTable = new TranslationEntry[numPages];
    ///pageTable[i].physicalPage = i;
    /// 说明物理帧的分配总是循环变量i的值。可以想象，当两个程序同时驻留内
    ///存时后一个程序会装入到前一个程序的物理地址中，从而将先前已装入的程序覆盖。
    ///可见基本的Nachos并不具有多个程序同时驻留内存的功能。
    ///为了实现多个程序同时驻留内存的功能需要我们改进Nachos的内存分配算法的设计。
    /// 一个比较简单的设计就是利用Nachos在../userprog/bitmap.h中文件定义的Bitmap类。
    /// 利用bitmap记录和申请内存物理帧，使不同的程序装入到不同的物理空间中去。
    for (i = 0; i < numPages; i++) {
   pageTable[i].virtualPage = i;  // for now, virtual page # = phys page #
   pageTable[i].physicalPage = i;
   pageTable[i].valid = TRUE;
   pageTable[i].use = FALSE;
   pageTable[i].dirty = FALSE;
   pageTable[i].readOnly = FALSE;  // if the code segment was entirely on a separate page, we could set its pages to be read-only
    }
    
// zero out the entire address space, to zero the unitialized data segment and the stack segment
///整个地址空间全部归零，也就是单道程序设计，虚拟空间和物理空间之间的映射是线性的
    bzero(machine->mainMemory, size);

// then, copy in the code and data segments into memory
///在创建地址空间之后，程序(可执行文件)被加载到空间中。此处使用noff文件
    if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
         noffH.code.virtualAddr, noffH.code.size);
        executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]),
         noffH.code.size, noffH.code.inFileAddr);
    }
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
         noffH.initData.virtualAddr, noffH.initData.size);
        executable->ReadAt(&(machine->mainMemory[noffH.initData.virtualAddr]),
         noffH.initData.size, noffH.initData.inFileAddr);
    }

}
```

#### 目前的处理方法存在的问题

在 Nachos 中，核心线程不需要单独分配内存，而利用 `Thread::Fork()` 创建的线程，只需调用 `Thread::StackAllocate() `为其分配栈空间，在几个 CPU 寄存器中设置线程入口 `Thread::Root()`，线程的执行体，以及线程知悉结束时需要做的工作（线程结束时调用 `Thread::Finish()`）。

如果根据常规的实现方法，应用程序运行时，系统应该首先为应用程序分配一个 PCB，存放应用程序进程的相应信息，进程号可以是 PCB 数组的索引号。然后再根据应用程序的文件头计算所需的实页数，根据内存的使用情况为其分配足够的空闲帧，将应用程序的代码与数据读入内存所分配的帧中，创建页表，建立虚页与实页的映射关系，然后再为应用程序分配栈与堆并且在PCB中建立打开文件的列表，列表的索引即为文件描述符。最后将 pid、页表位置、栈位置及堆位置等信息记录在 PCB 中，在 PCB 中建立三个标准设备的映射关系，并记录进程与线程的映射关系，以及进程的上下文等。

并且常规的实现方法中，进程被创建后不应立即执行，应该将程序入口等记录到 PCB 中，一旦相应的核心线程引起调度，就从 PCB 中获取所需的信息来执行该进程。

然后目前 Nachos 并不是这样实现，Nachos 的实现较为简单，没有显式定义 PCB，而是将进程信息分散到相应的类对象中；例如利用所分配内存空间的对象指针表示一个进程，该对象中含有进程的页表、栈指针以及与核心线程的映射等信息。进程的上下文保存在核心线程中，当一个线程被调度执行后，依据线程所保存的进程上下文中执行所对应的用户进程。




&nbsp;

### Nachos Filesystem

Nachos中有两个版本的文件系统。"stub"版本只是Unix文件系统的前端，这样用户就可以访问Nachos中的文件而不必编写自己的文件系统。第二个版本允许我们在原始磁盘上实现一个文件系统，这实际上是一个普通的Unix文件，只能通过模拟磁盘访问。模拟磁盘通过读取和写入单个扇区的操作专门访问底层Unix文件。两个文件系统提供相同的服务和接口。

文件是通过几层对象访问的。在最低级别，`machine/Disk`对象提供了用于启动单个扇区的IO操作的粗略接口。在Disk之上，`SynchDisk`对象提供对磁盘的同步访问，阻塞调用直到请求的操作实际完成。此外，`SynchDisk`允许多个线程安全地并发调用磁盘操作。`FileSystem`对象处理单个文件的创建、删除、打开和关闭。在文件系统旁边，`OpenFile`对象处理单个文件内容的访问，允许查找、读取和写入。`FileSystem::Open`例程返回一个文件描述符（实际上是一个`OpenFile*`），用于访问文件的数据。

#### synchdisk

这个数据结构用于同步访问物理磁盘。物理磁盘是一个异步设备，即磁盘请求会立刻反应，然后发生中断。这个数据结构是位于磁盘之上的一层，为磁盘访问提供接口。因为物理磁盘一次只能处理一个操作，我们需要使用信号量和锁来使中断处理程序和挂起的请求同步，实现互斥。

`SynchDisk`对象位于原始磁盘上方，为其服务提供了更清晰的界面。具体来说，它提供了阻塞调用线程直到相应的IO完全中断发生之后的操作。相反,`Disk`对象提供了启动IO操作的机制，但不提供在请求完成之前阻止调用方的方便方法。此外，`SynchDisk`提供了互斥，以便多个线程可以安全地并发调用`SynchDisk`例程。所以。大多数应用程序会发现使用`SynchDisk`是合适的，而不是直接使用`Disk`对象。

类的私有变量有三个，分别为原始的物理磁盘、信号量和锁。

构造函数中，给信号量赋值为0，这样只能一个线程访问该信号量，新建互斥锁，并创建一个磁盘对象。

```cpp
SynchDisk::SynchDisk(const char* name)
{
    semaphore = new Semaphore("synch disk", 0);
    lock = new Lock("synch disk lock");
    disk = new Disk(name, DiskRequestDone, (_int) this);
}
```

`ReadSector`方法用于将磁盘中的内容读入缓存中，在读之前需要先请求锁，请求后将指定扇区内的数据读给data，然后等待中断信号量后，将锁释放。`WriteSector`方法将缓存中的数据写入磁盘对应扇区，也是同样先请求锁，写完后等待中断信号，最后释放锁。

```cpp
void SynchDisk::ReadSector(int sectorNumber, char* data)
{
    lock->Acquire();         // only one disk I/O at a time
    disk->ReadRequest(sectorNumber, data);
    semaphore->P();          // wait for interrupt
    lock->Release();
}
void SynchDisk::WriteSector(int sectorNumber, char* data)
{
    lock->Acquire();			// only one disk I/O at a time
    disk->WriteRequest(sectorNumber, data);
    semaphore->P();			// wait for interrupt
    lock->Release();
}
```

`DiskRequestDone`函数是一个磁盘中断处理器，用于访问完磁盘后释放资源，所调用的`RequestDone`方法内就是对信号量资源的释放，可以唤醒其他等待磁盘请求的线程。

```cpp
static void DiskRequestDone (_int arg)
{
    SynchDisk* dsk = (SynchDisk *)arg; // disk -> dsk

    dsk->RequestDone();                // disk -> dsk
}
void SynchDisk::RequestDone()
{
    semaphore->V();
}
```

#### filehdr

这个数据结构是用来管理硬盘文件头的。文件头描述了在硬盘的何处可以找到文件的数据，以及文件的其他信息，如长度、所有者等。文件头被组织成一个简单的数据块指针表。文件头数据结构可以存储在内存或磁盘上。当它在磁盘上时，它存储在单个扇区中——这意味着我们假设该数据结构的大小与一个磁盘扇区相同。如果没有间接寻址，这将把最大文件长度限制在略低于4K字节。这个类没有构造函数，相反，文件头可以通过为文件分配块来初始化(如果它是一个新文件)，或者通过从磁盘读取来初始化。

文件头的私有参数有三个，分别表示文件的比特数（大小）、文件所占的扇区的数量和文件中每个数据块的磁盘扇区号。可以看出，这些其实不是文件的实际数据，可能更多只是一个描述，实际的内容还是存在磁盘中，`dataSectors`中只是虚拟地址，真实的物理地址还需要经过映射得到。

```cpp
private:
  int numBytes;          // Number of bytes in the file
  int numSectors;        // Number of data sectors in the file
  int dataSectors[NumDirect];
```

第一个函数`Allocate`，用于分配文件头的内存空间，特指是为新创建的文件分配文件头空间。这里使用bitmap来标识空闲的扇区。如果没有足够的空闲block来容纳新的文件头的话，返回False，即寻找bitmap是否还有空闲项，如果有的话就把bitmap的对应位索引赋值给文件数据块的扇区号。与此相反，`Deallocate`函数用于释放文件头的内存空间，即清空文件数据块的扇区号中存储的所有索引。

```cpp
bool FileHeader::Allocate(BitMap *freeMap, int fileSize)
{
    numBytes = fileSize;
    numSectors  = divRoundUp(fileSize, SectorSize);
    if (freeMap->NumClear() < numSectors)
   return FALSE;     // not enough space

    for (int i = 0; i < numSectors; i++)
   dataSectors[i] = freeMap->Find();
    return TRUE;
}
void FileHeader::Deallocate(BitMap *freeMap)
{
    for (int i = 0; i < numSectors; i++) {
	ASSERT(freeMap->Test((int) dataSectors[i]));  // ought to be marked!
	freeMap->Clear((int) dataSectors[i]);
    }
}
```

`FitchFrom`和`WriteBack`分别是根据扇区号从磁盘中读取文件内容和将文件内容写入磁盘，更详细的实现在`synchdisk`中。

```cpp
void FileHeader::FetchFrom(int sector)
{
    synchDisk->ReadSector(sector, (char *)this);
}
void FileHeader::WriteBack(int sector)
{
    synchDisk->WriteSector(sector, (char *)this);
}
```

`ByteToSector`函数用于求每一具体比特所存储的扇区号，输入参数即为需要的比特号（偏移），这实际上是从虚拟地址(文件中的偏移量)到物理地址(存储偏移量处的数据的扇区)的转换。其实也很简单，只需要把比特号和每一个扇区的大小相除取整即可。

```cpp
int FileHeader::ByteToSector(int offset)
{
    return(dataSectors[offset / SectorSize]);
}
```

`print`方法打印文件头的内容，以及文件头指向的数据块中存储的所有内容。

```cpp
void FileHeader::Print()
{
    int i, j, k;
    char *data = new char[SectorSize];

    printf("FileHeader contents.  File size: %d.  File blocks:\\\\n", numBytes);
    for (i = 0; i < numSectors; i++)
   printf("%d ", dataSectors[i]);
    printf("\\\\nFile contents:\\\\n");
    for (i = k = 0; i < numSectors; i++) {
   synchDisk->ReadSector(dataSectors[i], data);
        for (j = 0; (j < SectorSize) && (k < numBytes); j++, k++) {
       if ('\\\\040' <= data[j] && data[j] <= '\\\\176')   // isprint(data[j])
      printf("%c", data[j]);
            else
      printf("\\\\\\\\%x", (unsigned char)data[j]);
   }
        printf("\\\\n");
    }
    delete [] data;
}
```

#### openfile

这个数据结构是为了打开、关闭、读取和写入单个Nachos文件。与UNIX一样，必须先打开文件，然后才能对其进行读写。一旦我们都完成了，我们可以关闭它(在Nachos中，通过删除OpenFile数据结构)。与UNIX中一样，为了方便，我们在文件打开时将文件头保存在内存中。

文件下有两种实现方式，第一种是将文件操作转化为底层的Unix操作完成，即使用Unix的api来完成，分别定义了读、写、读某个位置、写某个位置、返回长度的函数，这种方法不再具体分析。

第二种实现是自己手动实现，当一个文件打开时，需要把文件头带入内存中。

头文件中：

```cpp
int ReadAt(char *into, int numBytes, int position) { 
		Lseek(file, position, 0); 
	return ReadPartial(file, into, numBytes); 
	}	
```

```cpp
//----------------------------------------------------------------------
// Lseek
// 	Change the location within an open file.  Abort on error.
//----------------------------------------------------------------------

void 
Lseek(int fd, int offset, int whence)
{
    int retVal = lseek(fd, offset, whence);
    ASSERT(retVal >= 0);
}
```

构造函数输入文件头在磁盘上的位置，调用文件头的`FetchFrom`方法，底层是调用synchdisk的`ReadSector`，从而可以读取扇区内容，同时将指针指向文件开头。

```cpp
OpenFile::OpenFile(int sector)
{
    hdr = new FileHeader;
    hdr->FetchFrom(sector);
    seekPosition = 0;
}
```

`ReadAt`和`WriteAt`方法都是从position点开始进行对应操作，然后返回对应操作经历的位数。前者具体实现是：先判断是否越界（超出范围），然后根据位置去求当前位置所在的扇区，并根据剩下的长度求出最后的扇区位置，然后将这两扇区中间的所有数据进行读取，读取到buf缓存中，然后在缓存中选取我们想要的部分进行复制，最后返回经历的长度；后者大体类似，不同之处在于先读取需要写的数据到缓存中，然后把需要改写的数据复制给缓存中的数据，再进行一个写回操作。

```cpp
int OpenFile::ReadAt(char *into, int numBytes, int position)
{
    int fileLength = hdr->FileLength();
    int i, firstSector, lastSector, numSectors;
    char *buf;

    if ((numBytes <= 0) || (position >= fileLength))
       return 0;           // check request
    if ((position + numBytes) > fileLength)
   numBytes = fileLength - position;
    DEBUG('f', "Reading %d bytes at %d, from file of length %d.\\\\n",
         numBytes, position, fileLength);

    firstSector = divRoundDown(position, SectorSize);
    lastSector = divRoundDown(position + numBytes - 1, SectorSize);
    numSectors = 1 + lastSector - firstSector;

    // read in all the full and partial sectors that we need
    buf = new char[numSectors * SectorSize];
    for (i = firstSector; i <= lastSector; i++)
        synchDisk->ReadSector(hdr->ByteToSector(i * SectorSize),
               &buf[(i - firstSector) * SectorSize]);

    // copy the part we want
    bcopy(&buf[position - (firstSector * SectorSize)], into, numBytes);
    delete [] buf;
    return numBytes;
}
int OpenFile::WriteAt(char *from, int numBytes, int position)
{
    int fileLength = hdr->FileLength();
    int i, firstSector, lastSector, numSectors;
    bool firstAligned, lastAligned;
    char *buf;

    if ((numBytes <= 0) || (position >= fileLength))  // For original Nachos file system
//    if ((numBytes <= 0) || (position > fileLength))  // For lab4 ...
	return 0;				// check request
    if ((position + numBytes) > fileLength)
	numBytes = fileLength - position;
    DEBUG('f', "Writing %d bytes at %d, from file of length %d.\\\\n",
			numBytes, position, fileLength);

    firstSector = divRoundDown(position, SectorSize);
    lastSector = divRoundDown(position + numBytes - 1, SectorSize);
    numSectors = 1 + lastSector - firstSector;

    buf = new char[numSectors * SectorSize];

    firstAligned = (bool)(position == (firstSector * SectorSize));
    lastAligned = (bool)((position + numBytes) == ((lastSector + 1) * SectorSize));

// read in first and last sector, if they are to be partially modified
    if (!firstAligned)
        ReadAt(buf, SectorSize, firstSector * SectorSize);
    if (!lastAligned && ((firstSector != lastSector) || firstAligned))
        ReadAt(&buf[(lastSector - firstSector) * SectorSize],
				SectorSize, lastSector * SectorSize);

// copy in the bytes we want to change
    bcopy(from, &buf[position - (firstSector * SectorSize)], numBytes);

// write modified sectors back
    for (i = firstSector; i <= lastSector; i++)
        synchDisk->WriteSector(hdr->ByteToSector(i * SectorSize),
					&buf[(i - firstSector) * SectorSize]);
    delete [] buf;
    return numBytes;
}
```

`Seek`、`Read`和`Write`方法分别是跳转定位、读和写，读写基于上面的ReadAt和WriteAt，读写的同时将指针前进。

```cpp
void OpenFile::Seek(int position)
{
    seekPosition = position;
}
int OpenFile::Read(char *into, int numBytes)
{
   int result = ReadAt(into, numBytes, seekPosition);
   seekPosition += result;
   return result;
}

int OpenFile::Write(char *into, int numBytes)
{
   int result = WriteAt(into, numBytes, seekPosition);
   seekPosition += result;
   return result;
}
```

#### directory

directory是用于管理类似unix的文件名目录的数据结构。目录其实是一个形如`<file name, sector>`的表，根据目录中每个文件的名称，可以找到磁盘上其文件头的位置。为了方便起见，我们规定文件名长度应该小于等于9个字符。但是这个文件系统的定义存在一个问题，就是文件目录的长度是固定的，即一旦所有的条目都被使用，将无法创建更多的文件。

首先定义一个`DirectoryEntry`类，即一个目录条目，表示目录中的一个文件。每个条目都给出文件名称以及文件头文件在磁盘上的位置。`isUse`表示该目录条目是否在使用中，`sector`表示该文件头文件磁盘位置，`name`表示文件名。

```cpp
class DirectoryEntry {
  public:
    bool inUse;
    int sector;
    char name[FileNameMaxLen + 1];
};
```

接着定义文件类`directory`，目录中的每个条目都表示一个文件，并描述了在哪里可以找到它们。目录结构可以被存储在内存中或磁盘中，作为常规的Nachos文件存储。

首先是初始化工作，定义一个固定长度的条目数组，并标记每个条目都未被使用。

```cpp
Directory::Directory(int size)
{
    table = new DirectoryEntry[size];
    tableSize = size;
    for (int i = 0; i < tableSize; i++)
	table[i].inUse = FALSE;
}
```

`FetchFrom`函数将文件内容从磁盘上读入到文件目录中；`WriteBack`	函数相反，将文件目录上的修改写回到磁盘上。

```cpp
void Directory::FetchFrom(OpenFile *file)
{
    (void) file->ReadAt((char *)table, tableSize * sizeof(DirectoryEntry), 0);
}
void Directory::WriteBack(OpenFile *file)
{
    (void) file->WriteAt((char *)table, tableSize * sizeof(DirectoryEntry), 0);
}
```

`FindIndex`函数根据文件名在文件目录中寻找对应元素，如果文件名合法并存在相同文件名项，则返回项的索引，如果不存在则返回-1。`Find`函数与FindIndex函数类似，只是返回值变成了对应的文件

```cpp
int Directory::FindIndex(char *name)
{
    for (int i = 0; i < tableSize; i++)
        if (table[i].inUse && !strncmp(table[i].name, name, FileNameMaxLen))
       return i;
    return -1;    // name not in directory
}
int Directory::Find(char *name)
{
    int i = FindIndex(name);

    if (i != -1)
	return table[i].sector;
    return -1;
}
```

`Add`方法用于向目录中加入一个文件，如果加入成功则返回True，如果有相同文件名或目录已满，则返回False。Add函数的参数也需要包含`newSector`属性，用于说明所添加文件的头文件所在硬盘位置。

`Remove`函数用于从目录中删除文件，成功返回True，若文件名不存在则返回False。

```cpp
bool Directory::Remove(char *name)
{
    int i = FindIndex(name);

    if (i == -1)
   return FALSE;     // name not in directory
    table[i].inUse = FALSE;
    return TRUE;
}
void Directory::List()
{
   for (int i = 0; i < tableSize; i++)
	if (table[i].inUse)
	    printf("%s\\\\n", table[i].name);
}
```

`List`函数和`Print`函数都是打印作用，前者是打印当前目录中所有的文件名，后者是显示当前目录中文件的所有信息，包括文件名、文件头位置和文件内容。

```cpp
void Directory::List()
{
   for (int i = 0; i < tableSize; i++)
	if (table[i].inUse)
	    printf("%s\\\\n", table[i].name);
}
void Directory::Print()
{
    FileHeader *hdr = new FileHeader;

    printf("Directory contents:\\\\n");
    for (int i = 0; i < tableSize; i++)
   if (table[i].inUse) {
       printf("Name: %s, Sector: %d\\\\n", table[i].name, table[i].sector);
       hdr->FetchFrom(table[i].sector);
       hdr->Print();
   }
    printf("\\\\n");
    delete hdr;
}
```

#### filesys

这个数据结构代表了Nachos的文件系统，它有两个关键参数，分别是为根目录和用于分配磁盘扇区的位图。这两个关键参数都被视为普通的文件，它们的文件头在特殊的扇区0和1，所以在操作系统初始化时就可以被文件系统找到。

在构造函数中，如果format为true，则初始化根目录和位图；如果为false，则只是单纯打开这两个文件。

`Create`方法用于在文件系统中创建文件。由于我们无法动态改变文件大小，所以需要在初始化时给定文件大小。先检查文件名是否合法，然后为文件分配磁盘上的数据空间，将文件名添加到目录中，在磁盘中存储新的文件头，将更新的位图和目录写回到磁盘中。

`Open`方法用于打开文件，需要通过目录找到文件的文件头，然后将文件头加载进内存。

`Remove`方法用于删除文件，需要在目录中删除文件名，删除文件头空间，删除文件数据，写回操作。

在Unix中，与已删除文件关联的磁盘扇区实际上不会返回到空闲列表或者空白内容，因为可能有进程已经打开了这个文件。只有在一个文件的所有打开的文件描述符都关闭之后，Unix才会解除分配与该文件关联的磁盘块，并将它们返回到空闲列表。Unix语义防止某些类型的不良情况。例如，当分页系统尝试从不存在的文件加载指令页时，删除二进制文件不能导致从该文件的文本段分页的进程意外终止。一个建议的Nachos分配是提供Unix风格的文件删除。

#### Putting It All Together

在磁盘上，可用扇区列表和顶级目录都存储在常规Nachos文件中。空闲列表存储为位图，每个扇区一位（例如，分配或空闲），文件本身存储在fnode 0中。因此，在文件系统中寻找一个需要读取与 fnode 0关联的文件的空闲扇区，使用位图函数定位到空闲扇区，然后将文件更改刷新回磁盘（通过适当的写回例程）以使更改永久化。

同样，顶级目录存储在与fnode 1关联的文件中。在创建新文件时更新目录需要读取与fnode1关联的文件，找到未使用的目录条目，初始化它，然后将目录写回磁盘。

创建文件系统时，系统会初始化`freelist`和顶级目录，然后打开包含这两个主要数据结构的文件。因此，可以通过`OpenFile*`变量`freeMapFile`和`directoryFile`访问空闲列表和目录（存储在磁盘上）的当前内容。

在创建和修改Nachos文件时，必须特别注意磁盘上的数据结构以及内存中的数据结构。例如，当创建文件时必须分配一个新的`FileHeader`，与扇区一起保存数据（空闲列表文件）。顶层目录也必须更新。但是，在更新的空闲列表和目录文件刷新到磁盘之前，这些更改不会成为永久性的。

如果跟踪`FileSystem::Create`的代码，则会看到所有的分配和更新都是首先在调用线程本地的内存中进行的，只有在所有分配成功而没有错误之后，才会通过将它们提交到磁盘来永久更改。当（比如说）有足够的空闲块来创建文件，但是目录没有空间容纳新文件时，这种方法大大简化了错误恢复。通过不刷新磁盘成功的分配，创建实际上就不会生效，并且会直接取消整个事务。

另请注意，提供的文件系统代码假定在任何时候只有一个线程访问文件系统。具体来说，考虑如果两个线程试图同时创建一个文件会发生什么。一种情况是两个线程都获取当前的`freemap`，都更新它们的本地副本，然后都写入它们。充其量，只有一组更改将出现在磁盘上。在最坏的情况下，磁盘损坏。

&，现有的代码中没有试图通过将相关块集中在相邻扇区来减少磁盘延迟。

&nbsp;

#### 线程切换时都做了哪些工作

1. 先判断是否是用户线程，如果是的话保存状态。

2. 检查要切换下的线程栈是否溢出

3. 将调度的新线程设置为currentThread

4. 将新线程的状态设置为运行态

5. 切换新旧线程。在switch.s里有保存状态的汇编代码。switch 保存旧线程的信息，设置旧线程的PC指令，加载新线程的状态，保存新线程的返回地址。

6. 检查旧线程是否是因为执行完毕放弃CPU，如果是要销毁的线程，释放它的空间。

7. 再次判断是不是用户线程，是的话加载它的状态。

#### nachos系统同步磁盘是怎样实现的？为什么要有这样的同步磁盘机制？

Nachos中同步磁盘的机制是保证线程对磁盘访问的互斥性。

在Nachos只是一个模拟的操作系统，并不能对物理磁盘进行操作，所以Nachos系统的模拟磁盘其实是宿主机上的一个名为“DISK”的文件。

Disk类将“DISK”文件抽象成了一个磁盘，并向上层提供了两个接口：`ReadRequest(int sectorNumber, char* data)`和`void WriteRequest(int sectorNumber, char* data);`分别用于发出度读磁盘的请求和写磁盘的请求。通过阅读源代码可以看到，在`ReadRequest`和`WriteRequest`中，其实读写磁盘的过程完成之后，将一个中断加入到了中断等待队列中。

阅读源代码可以看到，在`ReadRequest`和`WriteRequest`返回之后，会执行`semaphore->P()`阻塞。其实这是和`ReadRequest`和`WriteRequest`中的注册中断过程是配套的，认真阅读代码可以发现磁盘中断主要做的事情其实就是`semaphore->V()`，这是模拟现实中磁盘I/O期间，发出请求的进程会阻塞，当I/O完成之后发出信号唤醒进程的过程。

但是Disk类并没有实现对磁盘的互斥访问，如果不加一定的机制，上述过程可能会出现一定的问题。我们考虑一种情形：A线程在完成写磁盘操作之后，阻塞在信号量P上，此时另一个线程B也能对磁盘进行修改，则可能将A线程写的内容给覆盖掉，这样就很容易出错。所以必须保证磁盘读写的互斥性。

同步磁盘syndisk其实就是给磁盘加了一把锁，将`Disk::ReadRequest`和`Disk::WriteRequest`进行了封装，在调用者两个函数前必须先抢锁，拿到锁之后才能对磁盘进行修改；而锁的释放操作则放在了`semaphore->()`之后，也就是必须一个磁盘请求完成之后才能处理下一个磁盘请求。这样就避免了由于对磁盘访问不互斥带来的问题。

&nbsp;

## Lab 2

1. 分析说明Nachos原有的线程调度策略。

2. 设计并实现具有静态优先级的非抢占式线程调度策略。

3. 以线程调试模式运行Nachos(./nachos -d t)，研究调试输出信息。上下文切换的次数与被测线程SimpleThread中打印输出的总行数一致吗？多余或缺少的上下文切换次数是什么原因造成的？请修改代码减少上下文切换的次数与被测线程SimpleThread中打印输出的总行数的差距。

4. 在实现了前面优先级调度的基础上，若要求实现优先级调度的老化(aging)，请给出在Nachos中实现的具体方法(不要求实现可运行的代码。在实验报告中用文字描述即可，必要时可在文字中结合关键代码片段、数据结构、对象等说明)。

## Lab3：使用信号量解决N线程屏障问题

### 实验内容

1. 分析说明Nachos的信号量是如何实现的。

2. 在Nachos中是如何创建及运行并发(而非线程自己主动调用Yield放弃CPU)线程的。

3. 先按“The Little Book of Semaphores”中3.6.4小节中的代码实现N线程屏障。用不同的随机数种子seed测试(./nachos -rs seed)，是否会发现有可能多个线程均判定自己为最后一个到达的线程，这个现象是什么原因造成的？该现象会导致N线程屏障出现与有题目要求不一致的错误码？

4. 请修改代码消除上面3中出现的现象。

5. 用不同的随机数种子测试，是否会发现各线程打印输出的rendezvous行的顺序，基本就是线程被创建的顺序(0,1,2…9)的现象？这是为什么，难道-rs选项没有起作用？试验在打印输出rendezvous之前加延迟(用软件空循环耗时)或Linux的sleep能否解决此问题，并解释为什么。

6. 请试着修改代码解决上面5出现的现象。提示：不用修改Nachos的核心实现代码，修改的是我们编写的N线程屏障的代码。

### 实验与机制分析

#### 分析说明Nachos的信号量是如何实现的

`monitor`下给出了Nachos对于信号量等机制的实现，其中对于信号量的实现在`synch.cc`与`synch.h`中。

头文件中给出了对于信号量的定义：

```c
class Semaphore {
  public:
    Semaphore(const char* debugName, int initialValue);    // set initial value
    ~Semaphore();                  // de-allocate semaphore
    char* getName() { return name;}          // debugging assist
    
    void P();   // these are the only operations on a semaphore
    void V();   // they are both *atomic*
    
  private:
    char* name;  // useful for debugging
    int value;         // semaphore value, always >= 0
    List *queue;       // threads waiting in P() for the value to be > 0
};
```

实现机制如下：

对于每个信号量S，维护一个用于控制的整型变量初始化，给与它一个非负数的整数值。

执行P（wait(S)），信号标S的值将被减少。企图进入临界区段的进程，需要先执行P（wait(S)）。当信号标S减为负值时，进程会被挡住，不能继续；当信号标S不为负值时，进程可以获准进入临界区段。

执行V（signal(S)），信号标S的值会被增加。结束离开临界区段的进程，将会执行V（signal(S)）。当信号标S不为负值时，先前被挡住的其他进程，将可获准进入临界区段。

每执行一次P操作，意味着请求的进程分配到一个资源；每执行一次V操作，意味着进程释放了一个资源

其中，对于`P()`：如果S的`value`大于0，则将其减一，否则认为资源不足（需要等待）而添加到等待队列中并将线程`Sleep()`。对于 `V()`：从等待队列的**队首**取出一个线程，若真的有线程（队列不为空），则将其设置为就绪态，`value`加一。同时，**为保证`P()`、`V()`都是原子操作，这两个函数都由关中断和开中断括起**。

#### 在Nachos中是如何创建及运行并发(而非线程自己主动调用Yield放弃CPU)线程的

在Nachos中创建及运行并发线程，可以通过信号量来实现（如使用`mutex = new Semaphore("mutex", n)`）。

当n个线程已经占用该资源且都没有释放，第`n+1`个并发线程试图通过`P()`时就会发现`value=0`而调用`Sleep()`使得自身进入阻塞状态，待有其他线程释放资源`V()`并通过`Yield()`放弃CPU时就会将其从队列中取出并置为就绪态参与调度。

#### 实现N线程屏障

The Little Book of Semaphores - 3.6.4 中对该问题的处理如下：

<img src="OS课设\36.png" style="zoom:80%;" />

其中我们定义了两个信号量：互斥信号量`mutex`与资源信号量`barrier`。对于n个线程，每个线程都会使得`count`加一，当`count`小于n，即不足n个线程执行时，会调用`wait()`阻塞当前线程，当第n个线程到达后，`barrier`调用`signal()`函数并通过该函数**依次**唤醒就绪队列中阻塞的线程，最终n个线程几乎同时开始运行。

##### 代码实现

参照`n3readme.txt`中的代码结构实现如下：

```c
void BarThread(_int which){
    MakeTicks(N_TICKS);
    printf("Thread %d rendezvous\n", which);

    mutex->P();
    count=count+1;
    mutex->V();
    
    if(count==N_THREADS)
    { barrier->V();
        printf("Thread %d is the last\n",which);
    }

    barrier->P();
    barrier->V();
    printf("Thread %d critical point\n", which);
}

void ThreadsBarrier(){
    char s[N_THREADS][16];
    
    mutex = new Semaphore("mutex",1);
    barrier = new Semaphore("barrier",0);

    // Create and fork N_THREADS threads 

    for(int i = 0; i < N_THREADS; i++) {
		sprintf(s[i], "%d", i);
        threads[i] = new Thread(s[i]);       
        threads[i]->Fork(BarThread, i);
    }

}
```

##### 问题与分析

> 有可能多个线程均判定自己为最后一个到达的线程

<img src="OS课设\37.png" style="zoom:40%;" />

如图，线程9、1、5就出现了这种问题。

> 一个疑问：只有在rs=12时特别容易出现这种错误，其他值则很正常，老师说这是一个偶然，也有少数其他几个数会触发

我们可以假设这样的场景对问题进行还原：前9个线程执行完`mutex->V();`后不再占用时间片，此时第10个线程将`count`加到n，打印`Thread  is the last`并将自己挂起，此时若调度到前9个线程中的任意一个继续执行代码都会认为自己也是最后一个线程，根本原因在于**全局变量在所有线程之间共享**，所以所有线程对count都共享，且对count的判断不是互斥的。

这就会导致某些情况下有多个线程认为线程数已经到n，从而导致实际上线程数还不到n的时候所有线程就被允许调度执行。

##### 修正

所以我们只需要将对count的判断加到互斥代码块中即可

```c
void BarThread(_int which){   

    printf("Thread %d rendezvous\n", which);
    
    waiter->P();
    count=count+1;
    if(count==N_THREADS)    
     { accepter->V();
       printf("Thread %d is the last\n",which);
     }
    waiter->V();
  
   
    accepter->P();
    accepter->V();
    printf("Thread %d critical point\n", which);
}
```

<img src="OS课设\38.png" style="zoom:40%;" />

#### 耗时问题

空循环与耗时并不能解决问题，对于如下代码：

```c
void MakeTicks(int n)
{
//    sleep(1)
    for(int i = 0; i<n;i++){}   
}
```

<img src="OS课设\39.png" style="zoom:40%;" />

根本原因在于这里的空循环耗时和sleep函数都不是借助时钟前进而完成随机中断和线程切换的，它们只能算是连续执行的一个程序，没有对中断的判断和处理，所以起不到作用。

所以我们可以直接调用`OneTick()`：

```c
void MakeTicks(int n){  // advance n ticks of simulated time
    for(int i = 0; i<n;i++){
        interrupt->OneTick();
    } 
}
//当然，先要
//#include "system.h"
//#include "syscall.h"
```

当然，也可以使用`time`：

```c
void MakeTicks(){
   time_t start_t = time(NULL);
   while(difftime(time(NULL), start_t)<N_TICKS);
}
```

&nbsp;



## Lab6：系统调用与多道用户程序

### 实验内容

1. 扩展现有的`class AddrSpace`的实现，使得Nachos可以实现多道用户程序。

2. 按照实验指导书中的方法，完成`class AddrSpace`中的Print函数。

3. 实现Nachos系统调用：`Exec()`，一个用户程序启动另一个用户程序。注意本实验要求实现的`Exec()`系统调用，是**在另一个地址空间运行指定的另一个用户程序，新程序并没有覆盖调用者的地址空间**。这与Unix/Linux的系统调用`exec()`不同。

4. 在Nachos中增加并实现一个新的系统调用：`PrintInt()`，在**用户程序**中打印一个整数值。

5. 在实现了多道用户程序的基础上，若要求在Nachos中实现与Unix/Linux 的`fork()/exec()`功能类似的Nachos系统调用`Fork()/Exec()`，及写时复制 (copy-on-write) 机制，请给出在Nachos中**实现的具体方法**(实现时假定有足够的物理内存，无需页面置换。不要求实现可运行的代码。在实验报告中用文字描述即可，必要时可在文字中结合关键代码片段、数据结构、对象等说明)。

注1：实现PrintInt系统调用需要修改`code/userprog/syscall.h`及`code/test/start.s`。在对原文件备份后，这两个文件可**原位修改**。

Lab6涉及2个用户进程，其源码文件分别命名为`code/test/exec.c`及`code/test/halt2.c`，其代码分别为：

```cpp
/* exec.c:  Simple program to running another user program. */
#include "syscall.h"

int main(){
    SpaceId pid;
    PrintInt(12345);
    pid = Exec("../test/halt2.noff");
    Halt();
    /* not reached */
}
```

```cpp
/* halt2.c:  Just do a "syscall" that prints a number and shuts down the OS. */
#include "syscall.h"

int main(){
    PrintInt(67890);
    Halt();
    /* not reached */
}
```

### 机制分析

#### 多道用户程序

通过`StartProcess()`函数（progtest.cc），内核线程转化成用户进程，线程控制块拥有了addr的信息，成为描述用户进程的进程控制块，最关键的信息就是`AddrSpace`对象，它保存了属于该进程的页表`TranslationEntry *pageTable`

关于用户进程的执行，由`machine`类负责，`machine`模拟了硬件部分，有cpu（靠`mipssim.cc`中`OneInstruction`函数的`switch`语句解释命令），用户寄存器组（数组），主存（char数组）。`machine`类还实现了一套关键函数，最重要的是Run（运行进程，被`StartProcess`函数首次调用），和`OneInstruction`(执行一条指令)。值得一提的是，每`OneInstruction`一次，就会调一次`OneTicks`函数，推动时钟前进，此时，用户态时钟才是准确的。

操作系统是靠中断驱动的，靠系统调用服务用户进程的。Nachos也不例外。`machine.h`中定义了所有异常类型，`syscall.h`中定义了所有系统调用接口。`machiyieldne`类里有`void RaiseException(ExceptionType which, int badVAddr);`函数，抛出异常后交由`exception.cc`中的`void ExceptionHandler(ExceptionType which);`处理，在这个异常处理函数中，根据异常种类which，调用interrupt类中的处理函数完成中断或系统调用的处理。题目中要求的`Exec()`系统调用也是这个流程。需要注意，参数，返回值等是存在预先设定好的寄存器中的。

Nachos是采用页式内存分配方式管理用户内存空间的，最主要的两部分，一是`AddrSpace`类中存了属于该进程的页表`TranslationEntry *pageTable`，并完成了页表初始化，内存空间分配；二是machine类中，`ExceptionType Translate(int virtAddr, int* physAddr, int size,bool writing);`函数模拟了mmu的工作，每次读写内存时，通过页表找到物理地址。原始的Nachos，物理地址等于虚拟地址，内存只能为一个用户进程服务，必须将用户进程所需空间全部分配内存。这样就会导致，在多道执行用户程序时，后创建的用户程序会覆盖之前的程序。

要实现多道程序，就需要有一个全局管理，来组织空闲主存，分配主存。我们完全可以使用[位图](https://www.zwn-blog.xyz/2022/06/28/os-note7-13/#%E7%A9%BA%E9%97%B2%E7%A9%BA%E9%97%B4%E7%AE%A1%E7%90%86)对空闲区域进行管理。例如：

将空闲空间用 位图（bit map） 或 位向量（bit vector） 表示，每块用一位说明是否为空闲，1 表示空闲，0 表示已经分配。

<img src="https://zwn2001.github.io/2022/06/28/os-note7-13/79.png" style="zoom:50%;" />


位图上方的值代表块号。

此方式查找磁盘上第 1 个空闲块和 n 个连续空闲块时简单高效：

- 按顺序检查位图中的每个字是否为 0 即可确定对应的块是否已经全部分配，第一个非 0 的字中，第一个 1 位偏移就对应着第一个空闲块；
- 连续 n 个空闲块只需要判断是否有连续 `[n / 字的位数]` 个字均为最大值（如一个字就是一个字节时，只要连续有 `[n / 8]` 个字节全为 255 就说明这部分块都空闲）。

除非整个位向量都能保存在内存中，否则位向量的效率不高。对于小磁盘，位向量的大小可以接受，但对于大磁盘而言（如 40GB，每块 1KB）就需要超过 5MB 空间存储位图。

这种方式更容易分配连续的块，这会使得访问文件的速度加快。但显然，缺点就是占用空间。

#### 实现Nachos系统调用：`Exec()`

每一条用户程序中的指令在虚拟机中被读取后，会被包装成一个`OneInstruction`对象，经过解码，在`mipssim`中的SWITCH语句中分别执行不同的操作。可以看出，系统调用是调用`syscall.h`中定义的一系列系统调用函数，而这些函数的实现是定义在`start.s`中的汇编代码。`start.s`中不同的系统调用实际上是把系统调用的类型放入二号寄存器，然后执行指令`syscall`指令。

在`machine/mipssim.cc`的534行我们可以看到，当指令是 syscall 时，它会运行`RaiseException`来处理异常。

> 应当注意这里不同于其他指令的处理方式，其没有使用break而是使用了return，而一般指令在break之后会进行一些其他处理。直接return造成的后果是没有对pc进行推进，那么程序就会再次读入这条系统调用操作，无限循环。所以我们后续还要手动添加这部分对pc推进的代码。

```cpp
  case OP_SYSCALL:
		RaiseException(SyscallException, 0);
		return; 
```

```cpp
machine/machine.cc
void Machine::RaiseException(ExceptionType which, int badVAddr)
{
 ...
 registers[BadVAddrReg] = badVAddr;
 DelayedLoad(0, 0);                  // finish anything in progress
 interrupt->setStatus(SystemMode);
 ExceptionHandler(which);            // interrupts are enabled at this point
 interrupt->setStatus(UserMode);
}
```

这个函数将控制从用户模式转移到 Nachos 内核，因为用户程序要么调用了系统调用，要么发生了一些异常(比如地址转换失败)。它调用 `ExceptionHandler` 来处理系统调用或异常，也就是说，我们需要做的就是在 `ExceptionHandler`中实现`Exec()`的逻辑。

该系统调用主要是根据给定的地址找到可运行文件然后生成一个用户进程来运行这个文件的一个过程。因此最主要的难点在于如何应用之前已经实现的函数来完成这一功能，最后采取 StartProcess 的实现思路来将用户进程映射到核心线程并分配 SpaceId 来完成该函数。

该系统调用的实现考察了对于用户程序执行过程的理解，以及针对地址空间分配的进一步改进。除此以外，就是引入了 SpaceId 来标记一个地址空间，并用该地址空间来标记一个线程，实现了 3 级标记的一个过程，类似于 pid 的功能。

该调用的功能是，从可执行文件name运行一个新的用户程序，并行执行，并返回新的程序的内存空间标识符`SpaceId`。

具体的实现思路：

- 从 4 号寄存器中可以获取 `Exec()` 的参数 `filename` 在内存中的地址（`addr = machine->ReadRegister(4)`）

- 利用 `Machine::ReadMem()` 从该地址读取应用程序文件名 `filename`

- 打开该应用程序（`OpenFile *executable = fileSystem->Open(filename)`）

- 为其分配内存空间、创建页表、分配 pid（`space = new AddrSpace(executable)`），至此为应用程序创建了一个进程。(因此在 AddrSpace 类中我们需要为进程分配 pid 以及对空闲帧进行管理，在进程退出时需要释放 pid，并释放为其所分配的帧。)

- 创建一个核心线程，并将该进程与新建的核心线程关联（`thread = new Thread(forkedThreadName)`），`thread->Fork(StartProcess,space->getSpace(ID)`)

此处需要注意，通过 `Thread::Fork() `创建的线程需要指明该线程要执行的代码（函数）及函数所需的参数；因此我们可以重载 `StartProcess(int spaceId)`，作为新建线程执行的代码，并将进程的 pid 传递给系统，供其它系统调用使用。

> 一个细节：通过2号寄存器返回pid的值，如果直接调用 `StartProcess(int spaceId)`同样可以实现指定的另一个用户程序，但无法返回pid，代码如下
>
> ```c++
>  int FilePathMaxLen = 100;
>  char filepath[FilePathMaxLen];
>  int fileaddr = machine->ReadRegister(4);
>  for (i = 0; filepath[i] != '\0'; i++) {
>  	if (!machine->ReadMem(fileaddr + 4 * i, 4, (int *) (filepath + 4 * i)))
> 		 break;
>  	}
>  extern void StartProcess(char *filename);
>  StartProcess(filepath);
> ```

##### Nachos 系统调用参数传递

Exec(FileName) 作为 Nachos 的系统调用，Exec(FileName) 执行时需要陷入到 Nachos 的内核中执行，因此需要将其参数 FileName 从用户地址空间传递（复制）到内核中，从而在内核中为 FileName 对应的应用程序创建相应的线程执行它。

一般来说，参数传递一共有 3 种方式。

- 通过寄存器
- 通过内存区域，将该内存区域的首地址存放在一个寄存器中
- 通过栈

在基于 MIPS 的架构中，对于一般的函数调用，通常利用 $4-$7 寄存器传递函数的前 4 个参数给子程序，参数多于 4 个时，其余的利用堆栈进行传递；

对于 Nachos 的系统调用，一般也是将要传递的参数依次保存到寄存器 \$4-\$7 中，然后根据这些寄存器中的地址从内存中读出相应的参数。


#### 增加并实现一个新的系统调用：`PrintInt()`

上文讲过，系统调用是调用`syscall.h`中定义的一系列系统调用函数，而这些函数的实现是定义在`start.s`中的汇编代码。`start.s`中不同的系统调用实际上是把系统调用的类型放入二号寄存器，然后执行指令`syscall`指令。也就是说，我们增加系统调用只需要在头文件中添加函数声明，在汇编代码中添加实现，在异常处理器中进行处理即可。

### 实现

#### 多道用户程序

定义位图对空闲页表进行管理：

```cpp
static BitMap *pageMap = new BitMap(NumPhysPages);
```
接下来我们修改虚实页分配的部分代码，对于每一个虚页，我们在位图中找一个未被分配的页面作为虚页映射。除此之外，我们需要保证在分配之前物理内存中空闲页面数量大于等于我们需要的页面数量，具体代码如下所示。

```cpp
for (i = 0; i < numPages; i++) {
	pageTable[i].virtualPage = i;  // for now, virtual page # = phys page #
	pageTable[i].physicalPage = pageMap->Find();   //use pageMap to manage 
	pageTable[i].valid = TRUE;
	pageTable[i].use = FALSE;
	pageTable[i].dirty = FALSE;
	pageTable[i].readOnly = FALSE; 
}

//初始化，但不要使用bzero(machine->mainMemory, size)，会清空整个主存
for (i = 0; i < numPages; i++){
	for (j = 0; j < PageSize; j++)
		machine->mainMemory[ pageTable[i].physicalPage * PageSize + j ] = 0;
}
```

分配完虚实页映射之后，我们需要将 noff 文件中的数据拷贝到 machine 的物理内存中，因此我们需要将虚拟地址所对应的物理地址求出，这里使用的是`machine->Translate`，具体求取过程如下所示。

```cpp
if (noffH.code.size > 0) {
	DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
   		noffH.code.virtualAddr, noffH.code.size);
	int phyAddr;
	machine->Translate(noffH.code.virtualAddr, &phyAddr, 4, TRUE);
    executable->ReadAt(&(machine->mainMemory[phyAddr]),  //may out of bound
  		noffH.code.size, noffH.code.inFileAddr);
}
if (noffH.initData.size > 0) {
	DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
   		noffH.initData.virtualAddr, noffH.initData.size);
	int phyAddr;
	machine->Translate(noffH.initData.virtualAddr, &phyAddr, 4, TRUE);
    executable->ReadAt(&(machine->mainMemory[phyAddr]),  //may out of bound
   		noffH.initData.size, noffH.initData.inFileAddr);
}
```

当然，也可以直接计算：

```cpp
    if (noffH.code.size > 0)
    {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",
              noffH.code.virtualAddr, noffH.code.size);
        int CodePhysicalAddress = pageTable[(noffH.code.virtualAddr / PageSize)].physicalPage * PageSize;
        executable->ReadAt(&(machine->mainMemory[CodePhysicalAddress]),
                           noffH.code.size, noffH.code.inFileAddr);
    }
   
    if (noffH.initData.size > 0)
    {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n",
              noffH.initData.virtualAddr, noffH.initData.size);
        int DataPhysicalAddress = pageTable[(noffH.initData.virtualAddr / PageSize)].physicalPage * PageSize
                + (noffH.initData.virtualAddr % PageSize);
        executable->ReadAt(&(machine->mainMemory[DataPhysicalAddress]),
                           noffH.initData.size, noffH.initData.inFileAddr);
    }
```



析构如下：

```cpp
AddrSpace::~AddrSpace()
{
   unsigned int i;
    pidMap->Clear(spaceId-100);
   for (i = 0; i < numPages; i++){    //reset the bitmap
      pageMap->Clear(pageTable[i].physicalPage);
   }
   delete [] pageTable;
}
```

#### 实现Nachos系统调用：`Exec()`

`exception.cc `中进行处理。

```cpp
void StartProcess(int spaceId){
    currentThread->space->InitRegisters();
    currentThread->space->RestoreState();
    machine->Run();
    ASSERT(FALSE);
}
```

```cpp
if ((which == SyscallException) && (type == SC_Exec)) {
    int FilePathMaxLen = 100;
    char filepath[FilePathMaxLen];
    int fileaddr = machine->ReadRegister(4);
    for (i = 0; filepath[i] != '\0'; i++) {
        if (!machine->ReadMem(fileaddr + 4 * i, 4, (int *) (filepath + 4 * i)))
            break;
    }
    OpenFile *executable = fileSystem->Open(filepath);
    if(executable == NULL) {
        printf("Unable to open file %s\n",filepath);
        return;
    }
    // 建立新地址空间
    AddrSpace *space = new AddrSpace(executable);
    delete executable; // 关闭文件
    // 建立新核心线程
    Thread *thread = new Thread(filepath);
    // 将用户进程映射到核心线程上
    thread->space = space;
    space->Print();
    thread->Fork(StartProcess,(int)space->getSpaceId());
    machine->WriteRegister(2,space->getSpaceId());
    {
        machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
        machine->WriteRegister(PCReg, machine->ReadRegister(PCReg) + 4);
        machine->WriteRegister(NextPCReg, machine->ReadRegister(PCReg)+4);
    }
}
```

#### 增加并实现一个新的系统调用：`PrintInt()`

start.s中添加`PrintInt`的MIPS代码

```MIPS
    .globl PrintInt
    .ent PrintInt
PrintInt:
    addiu $2, $0,SC_PrintInt ///将刚刚define的SC_PrintInt放到register2
    syscall                  ///MIPS machine会将参数存放到register4,5,6,7
    j       $31              ///执行system call
    .end PrintInt
```

system.h中添加方法定义与系统调用码

```cpp
#define SC_PrintInt 11
...
/*PrintINt*/
void PrintInt(int integer);
```

exception.cc中重写处理的方法对该系统调用进行处理。


```cpp
    int val = machine->ReadRegister(4); //将MIPS machine存的参数取出来
        interrupt->PrintInt(val);  //执行内核的system call
        {//以下将Program counter+4，否則会一直执行instruction
            machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
            machine->WriteRegister(PCReg, machine->ReadRegister(PCReg) + 4);
            machine->WriteRegister(NextPCReg, machine->ReadRegister(PCReg)+4);
        }
```

其中需要调用系统中断，在`interrupt.cc`中添加处理代码

```cpp
void Interrupt::PrintInt(int n)
{
    printf("==================================\n");
    printf("PrintInt:%d\n",n);
    printf("==================================\n");
}
```

### 实验结果

执行shell，将输出保存到`temp.log`：

<img src="OS课设\24.png" style="zoom:80%;" />

输出如下：

<img src="OS课设\25.png" style="zoom:80%;" />

&nbsp;

### Nachos系统调用`Fork()/Exec()`，及写时复制 (copy-on-write) 机制

> 部分代码参考 Linux 内核。https://github.com/torvalds/linux

#### `Fork()`

进程都是由其他进程创建出来的，每个进程都有自己的PID（进程标识号），在 Linux 系统的进程之间存在一个继承关系，所有的进程都是 init 进程（1号进程）的后代。可以通过 pstree 命令查看到进程的族谱。在 Linux 中所有的进程都通过 task_struct 描述，里面通过 parent 和 children 字段维护一个父子进程树的链表结构。

而 fork 函数是创建子进程的一种方法，它是一个系统调用函数，所以在看 fork 系统调用之前我们先来看看Linux下的 `system_call`。

Linux中系统调用处理函数 `system_call` 与 int 0x80 中断描述符表挂接。 `system_call` 是整个操作系统中系统调用软中断的总入口。所有用户程序使用系统调用，产生 int 0x80 软中断后，操作系统都是通过这个总入口找到具体的系统调用函数。

系统调用函数是操作系统对用户程序的基本支持。在操作系统中，像类似读盘、创建子进程之类的事物需要通过系统调用实现。系统调用被调用后会触发 int 0x80 软中断，然后由用户态切换到内核态（从用户进程的3特权级翻转到内核的0特权级），通过 IDT 找到系统调用端口，调用具体的系统调用函数来处理事物，处理完毕之后再由 iret 指令回到用户态继续执行原来的逻辑。

<img src="OS课设\21.png" style="zoom:60%;" />

然后拿到对应的 C 函数 `sys_fork`。因为会变中对应 C 的函数名在前面多加一个下划线，所以会跳转到 `_sys_fork` 处执行。

在 `_sys_fork` 中首先会调用 `find_empty_process` 申请一个空闲位置并获取一个新的进程号 pid。空闲位置由 `task[64]` 这个数组决定，也就是说最多只能同时 64 个进程同时在跑，并用全局变量 `last_pid` 来存放系统自开机以来累计的进程数，如果有空闲位置，那么 `++last_pid` 作为新进程的进程号，在 task[64] 中找到的空闲位置的 index 作为任务号。

`_sys_fork` 接下来调用 `copy_process` 进行进程复制：

1. 将 `task_struct` 复制给子进程，`task_struct` 是用来定义进程结构体，里面有关于进程所有信息；
2. 随后对复制来的进程结构内容进行一些修改和初始化赋0。比方说状态、进程号、父进程号、运行时间等，还有一些统计信息的初始化，其余大部分保持不变；
3. 然后会调用 copy_mem 复制进程的页表，但是由于Linux系统采用了写时复制(`copy on write`)技术，因此这里**仅为新进程设置自己的页目录表项和页表项，而没有实际为新进程分配物理内存页面**，此时新进程与其父进程共享所有物理内存页面。
4. 最后 GDT 表中设置子进程的 TSS（`Task State Segment`） 段和 LDT（`Local Descriptor Table`） 段描述符项，将子进程号返回。其中 TSS 段是用来存储描述进程相关的信息，比方一些寄存器、当前的特权级别等；

<img src="OS课设\22.png" style="zoom:60%;" />

需要注意的是子进程也会继承父进程的文件描述符，也就是子进程会将父进程的文件描述符表项都复制一份，也就是说如果父进程和子进程同时写一个文件的话可能产生并发写的问题，导致写入的数据错乱。

此外，在Nachos中所现有的文件系统是非常简略的，其中非常关键的一个问题就是没有PCB等相关的数据结构。

如果根据常规的实现方法，应用程序运行时，系统应该首先为应用程序分配一个 PCB，存放应用程序进程的相应信息，进程号可以是 PCB 数组的索引号。然后再根据应用程序的文件头计算所需的实页数，根据内存的使用情况为其分配足够的空闲帧，将应用程序的代码与数据读入内存所分配的帧中，创建页表，建立虚页与实页的映射关系，然后再为应用程序分配栈与堆并且在PCB中建立打开文件的列表，列表的索引即为文件描述符。最后将 pid、页表位置、栈位置及堆位置等信息记录在 PCB 中，在 PCB 中建立三个标准设备的映射关系，并记录进程与线程的映射关系，以及进程的上下文等。

并且常规的实现方法中，进程被创建后不应立即执行，应该将程序入口等记录到 PCB 中，一旦相应的核心线程引起调度，就从 PCB 中获取所需的信息来执行该进程。

然后目前 Nachos 并不是这样实现，Nachos 的实现较为简单，没有显式定义 PCB，而是将进程信息分散到相应的类对象中；例如利用所分配内存空间的对象指针表示一个进程，该对象中含有进程的页表、栈指针以及与核心线程的映射等信息。进程的上下文保存在核心线程中，当一个线程被调度执行后，依据线程所保存的进程上下文中执行所对应的用户进程。

#### `Copy-On-Write`

写时复制 `Copy-On-Write` 是一种延迟拷贝的资源优化策略，通常用在拷贝复制操作中，如果一个资源只是被拷贝但是没有被修改，那么这个资源并不会真正被创建，而是和原数据共享。因此这个技术可以推迟拷贝操作到首次写入之后进行。

fork 函数调用之后，这个时候因为`Copy-On-Write（COW）` 的存在父子进程实际上是**共享物理内存**的，并没有直接去拷贝一份，**kernel 把会共享的所有的内存页的权限都设为 `read-only`。当父子进程都只读内存，然后执行 `exec` 函数时就可以省去大量的数据复制开销**。

**当其中某个进程写内存时，内存管理单元 MMU 检测到内存页是 `read-only` 的，于是触发缺页异常（`page-fault`），处理器会从中断描述符表（IDT）中获取到对应的处理程序。在中断程序中，kernel就会把触发的异常的页复制一份，于是父子进程各自持有独立的一份，之后进程再修改对应的数据。这样我们就完成了写时复制**。



#### `Exec()`

当进程调用一种exec函数时，该进程完全由新程序代换，而新程序则从其main函数开始执行。我们所要做的是替换代码段和数据段，从磁盘加载新的代码段和数据段到物理内存，用页表映射回进程虚拟地址空间的代码段、数据段，同时应该将堆栈、命令行参数等变为最初状态，而且因为调用exec并不创建新进程，所以pid 不变、环境变量不变。如果当前运行的进程包含多个线程，那么所有线程都将被终止，新的进程映像将被加载并执行。没有终止当前进程线程的析构函数。

大致关键代码如下：

```c
void Interrupt::Exec() {
    //从寄存器获取文件名
    char filename[30];
    int addr=machine->ReadRegister(4);
    int i=0;
    do{
        machine->ReadMem(addr+i,1,(int *)&filename[i]);
    }while(filename[i++]!='\0');
    OpenFile *executable = fileSystem->Open(filename);

    NoffHeader noffH;
    unsigned int i, size;
    executable->ReadAt((char *) &noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) &&
        (WordToHost(noffH.noffMagic) == NOFFMAGIC))
        SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

    CountSizeAndPage();
    
    //将堆栈、命令行参数等变为最初状态
    InitStackAndShell();
    InitPageTable();
    
    //加载虚拟环境
    VM *vm = LoadVM();
    
    //磁盘加载新的代码段和数据段到物理内存，用页表映射回进程虚拟地址空间的代码段、数据段
    if (noffH.code.size > 0) {
        DEBUG('a', "\tCopying code segment, at 0x%x, size %d\n",
              noffH.code.virtualAddr, noffH.code.size);
        executable->ReadAt(&(virtualMemory_temp[noffH.code.virtualAddr]),
                           noffH.code.size, noffH.code.inFileAddr);
        vm->WriteAt(&(virtualMemory_temp[noffH.code.virtualAddr]),
                    noffH.code.size, noffH.code.virtualAddr*PageSize);
    }
    
    if (noffH.initData.size > 0) {
        DEBUG('a', "\tCopying data segment, at 0x%x, size %d\n",
              noffH.initData.virtualAddr, noffH.initData.size);
        executable->ReadAt(&(virtualMemory_temp[noffH.initData.virtualAddr]),
                           noffH.initData.size, noffH.initData.inFileAddr);
        vm->WriteAt(&(virtualMemory_temp[noffH.initData.virtualAddr]),
                    noffH.initData.size, noffH.initData.virtualAddr*PageSize);
    }
    
    //从其main函数开始执行
    machine->runFrom(main());

}
```

#### Reference

> 《深入理解计算机系统》
>
> 《Linux内核设计的艺术图解》

&nbsp;

## Lab7：虚拟内存

### 实验内容

1. 在未实现虚拟内存管理之前，Nachos在运行一个用户进程的时候，需要将程序运行所需全部内存空间一次性分配。虚拟内存实现将突破物理内存限制。本实验核心任务为根据理论学习中涉及的对换（Swapping）技术，在Lab6的基础上，设计并实现用户空间的虚拟内存管理。

2. 用户进程的帧数采用固定分配(建议5帧)，局部置换。

3. 实现“纯按需调页”(`pure demand paging`)。

4. 页置换算法可以采用LRU、增强型二次机会、二次机会、FIFO等算法之一，或自己认为合适的其他算法(不包括随机置换)。

5. 对`class Statistics`进行调用及修改，以便在程序结束时打出页故障次数及将牺牲页写入交换空间的次数。

6. 使用lab7目录中的示例程序n7(若lab7额外实现了多种算法，可用自己的lab7)，测试用户程序用同样ARRAYSIZE参数值的sort，但不同的页置换算法(详见code/lab7/n7readme.txt)多次运行n7。不同页置换算法运行结束时显示的user ticks数是否一样？解释这是为什么？

7. 最优页置换算法(OPT)有最低的页故障率，但需要未来的页面引用信息，因此不能用于实际环境，主要用于评估其他页置换算法的性能。在前述1-5实现的基础上，给出在Nachos中获得最优页置换算法页故障次数的具体实现方法(不要求实现可运行的代码。在实验报告中用文字描述即可，必要时可在文字中结合进关键代码片段、数据结构、对象等说明)。

### 机制分析

在Lab6中，我们使用`translarte.cc`中的`translate()`方法进行地址转换，在该方法的代码中会进行异常检测，如果出现Error则会返回相应的`Exception`，我们也可以发现比如在`WriteMem()`方法中就有这样一段：

```cpp
    exception = Translate(addr, &physicalAddress, size, TRUE);
    if (exception != NoException) {
		machine->RaiseException(exception, addr);
		return FALSE;
    }
```

在`translate()`方法中我们能注意到缺页异常如下：

```cpp
	} else if (!pageTable[vpn].valid) {
	    DEBUG('a', "virtual page # %d too large for page table size %d!\n", 
			virtAddr, pageTableSize);
	    return PageFaultException;
	}
```

所以与Lab6类似的，我们可以在`exception.cc`中`ExceptionHandler()`添加异常处理的实现。

对于页表结构 `TranslationEntry`：

```cpp
class TranslationEntry {
  public:
    int virtualPage;
    int physicalPage;
    bool valid;
    bool readOnly;
    bool use;
    bool dirty;
};
```

其中有效位valid 表示相关的页在进程的逻辑地址空间内，同时在内存中，可以访问。反之不可以访问（不合法或者在磁盘上）。

> 当然`machine`对象新建的时候会对主存进行内存分配与初始化，其实就是一个数组，`mainMemory = new char[MemorySize];`，在头文件中我们可以找到`#define MemorySize  (NumPhysPages * PageSize)`，而
>
> `#define NumPhysPages  32`，`#define PageSize  SectorSize`，其中`SectorSize`为128 。

在`exception.cc`中`ExceptionHandler()`添加异常处理的实现，逻辑就是进行中断，调用当前地址空间的页置换算法进行置换。

进程通过MMU试图访问页表中尚未调入内存中的页会触发页错误陷阱（page-fault trap），**操作系统（内核态）**会按如下流程处理页错误：

- 检查进程的内部页表（进程维护的内部页表和 PCB 一起保存）来确定该引用是否为合法地址，若引用非法则终止进程，否则准备从磁盘中调入页面；

- 操作系统寻找一个空闲帧（如从空闲帧链表中选择一个元素）；

- 操作系统调度 磁盘，将需要的页调入到上一步分配的空闲帧；

- 磁盘读操作完成后修改进程的内部页表和操作系统维护的页表以表示该页已经位于内存中（重置页表、填入帧号及修改页表标志位）；

- 重新读取因为页错误而中断的指令，进程可以访问此前无法访问的页。

 <img src="https://zwn2001.github.io/2022/06/28/os-note7-13/35.png" style="zoom:80%;" />

上述按需调页的一种极端情况是**在不调入任何需要页的情况下就执行进程**，因此进程执行过程中将不断出现页错误、调入页、继续执行。这种方案称为 **纯粹按需调页（pure demand paging）** 。

> 也就是说，初始时我们不会加载任何页到内存中，随着使用使得程序逐渐占满所分配的页（默认五页），此后进行页分配与置换。

理论上单个指令可能触发多个页错误（例如一页用于获取指令，一页用于获取指令所需的数据），但此种情况较少见，正常执行程序满足 **局部引用（locality of reference）** 特性，使按需调页的性能较好。

### 实现

添加系统调用，`start.s`：

```Text
    .globl PageFault
    .ent PageFault
PageFault:
    addiu $2, $0,SC_Page_Fault
    syscall
    j       $31
    .end PageFault
```

`syscall.h`：

```cpp
#define SC_Page_Fault	12
...
void PrintInt(int n);
```

`interrupt.h`：

```cpp
void PageFault(int badVAddr);
```

`interrupt.cc`：

```cpp
void Interrupt::PageFault(int BadVAddr)
{
    printf("badVAddr is %d\n",badVAddr);
    currentThread->space->FIFO(badVAddr);
    stats->numPageFaults++;
    machine->registers[NextPCReg]=machine->registers[PCReg];
    machine->registers[PCReg]-=4;
    printf("PCReg=%d,NextPCReg=%d\n",machine->registers[PCReg],machine->registers[NextPCReg]);
}
```

`exception.cc`：

```cpp
   else if(which==PageFaultException){
        int badVAddr=(int)machine->ReadRegister(BadVAddrReg);
        interrupt->PageFault(badVAddr);
    }
```

在地址空间中实现FIFO算法：

 `addspace.h` 中定义`void FIFO(int badVAdrr);`

```cpp
void AddrSpace::FIFO(int badVAddr) {
   printf("enter fifo\n");
    unsigned int oldPage = virtualMem[p_vm];
    unsigned int newPage;
    unsigned int tmp;
    newPage = badVAddr/PageSize;
    ASSERT(newPage < numPages);

    virtualMem[p_vm] = newPage;
    p_vm = (p_vm + 1) % MemPages;
    printf("physpage:old:%d,new:%d\n",oldPage,newPage);
    Swap(oldPage, newPage);
}
```

#### 虚拟内存的实现细节部分与踩坑

> Note that, when the position offset of "executable" is `noffH.code.inFileAddr`, where we need to write in virtual memory is `noffH.code.virtualAddr*PageSize`. Because in real file there is a header. But in virtual memory, there are only code and data segments!
>
> (If miss this will mess the instruction decode, and probabily will cause infinity loop!)

如何实现对换区？这是一个值得思考的问题。初始时我考虑使用原文件作为对换区，网上也有很多教程是这样写的，但其实并不完全对，对于原`noff`文件，在不熟悉其文件格式（毕竟老师没怎么讲且网上资料几乎没有）的情况下，对于文件头的部分进行对换似乎并没有太多问题，毕竟看上去就像是原位取出再原位写回，但这其实至少导致了两个问题，一方面，dirty的写回实际上导致了执行文件被修改，下次运行大概率不会按照正常逻辑执行，另一方面，用户分配的5页怎么办？如果是写在noff文件末尾，很容易导致文件所使用的的栈溢出导致文件损坏，比如noff文件头的magic number被覆盖。

我所选择的方式是建立一个文件模拟磁盘对换区的读写。代码如下

```cpp
AddrSpace::AddrSpace(OpenFile *executable) {
    //分配可用的spaceID，用位图也可以，不过有点不合适（逻辑上）
    for (int i = 0; i < 128; i++) {
        if (spaceIDs[i] == 0) {
            spaceID = i;
            spaceIDs[i] = 1;
            break;
        }
    }

    NoffHeader noffH;
    unsigned int i, size;
    executable->ReadAt((char *) &noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) &&
        (WordToHost(noffH.noffMagic) == NOFFMAGIC))
        SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStackSize;
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;
    //建立文件作为对换区，大小就是计算所得的size，因为noffH中只有code与initData是必要的，除此之外加上用户栈，就构成了所有必须的内存占用大小
    fileSystem->Remove("VirtualMemory");
    fileSystem->Create("VirtualMemory", size);

    InitPageTable();

    OpenFile *vm = fileSystem->Open("VirtualMemory");
    //virtualMemory_temp作为一个中间量
    char *virtualMemory_temp;
    virtualMemory_temp = new char[size];
    //初始化
    for (i = 0; i < size; i++)
        virtualMemory_temp[i] = 0;

    if (noffH.code.size > 0) {
        DEBUG('a', "\tCopying code segment, at 0x%x, size %d\n",
              noffH.code.virtualAddr, noffH.code.size);
        //将executable对应的code部分读入virtualMemory_temp，再从virtualMemory_temp读入vm中，只是为了理解方便，可以简化，下同
        executable->ReadAt(&(virtualMemory_temp[noffH.code.virtualAddr]),
                           noffH.code.size, noffH.code.inFileAddr);
        vm->WriteAt(&(virtualMemory_temp[noffH.code.virtualAddr]),
                    noffH.code.size, noffH.code.virtualAddr*PageSize);
    }
    if (noffH.initData.size > 0) {
        DEBUG('a', "\tCopying data segment, at 0x%x, size %d\n",
              noffH.initData.virtualAddr, noffH.initData.size);
        executable->ReadAt(&(virtualMemory_temp[noffH.initData.virtualAddr]),
                           noffH.initData.size, noffH.initData.inFileAddr);
        vm->WriteAt(&(virtualMemory_temp[noffH.initData.virtualAddr]),
                    noffH.initData.size, noffH.initData.virtualAddr*PageSize);
    }
    delete vm;

    print();
}
```

#### 对换操作的细节

对换时，如果是MemPages还没有分配完，就优先分配空页，否则从旧页中使用FIFO找一个牺牲页。

对换的关键就在于正确地读入与写回，如果读入的位置不正确就会导致程序无法正确执行，如果写回的位置不正确就会导致覆盖原有代码导致程序出错，值得注意的是，由于我们写入vm时按照的是虚拟内存位置，所以偏移量是`pageTable[oldPage].virtualPage * PageSize`，而读入内存是要按照物理内存的位置写入，所以内存读写的起始下标应该是`pageTable[newPage].physicalPage * PageSize]`。

```cpp
void AddrSpace::Swap(int oldPage, int newPage) {
    printf("enter swap\n");
    //新分配的页
    if(oldPage == -1){
        if(p_vm == 0){
            pageTable[newPage].physicalPage = MemPages - 1;
        } else{
            pageTable[newPage].physicalPage = p_vm - 1;
        }
    } else{
        //只有置换旧页时才可能写回
        WriteBack(oldPage);
        pageTable[newPage].physicalPage = pageTable[oldPage].physicalPage;
        pageTable[oldPage].physicalPage = -1;
        pageTable[oldPage].valid = false;
        pageTable[oldPage].use = false;
    }
    pageTable[newPage].valid = true;
    pageTable[newPage].use = true;
    pageTable[newPage].dirty = false;
    ReadIn(newPage);
    print();
}

void AddrSpace::WriteBack(int oldPage) {
    OpenFile *vm = fileSystem->Open("VirtualMemory");
    printf("enter writeBack\n");
    if (pageTable[oldPage].dirty) {
        printf("writeBack physpage:%d\n",oldPage);
        stats->numDiskWrites++;
        vm->WriteAt(&(machine->mainMemory[pageTable[oldPage].physicalPage * PageSize]), PageSize, pageTable[oldPage].virtualPage * PageSize);
        pageTable[oldPage].dirty = false;
    }
    delete vm;
}

void AddrSpace::ReadIn(int newPage) {
    printf("enter readin\n");
    OpenFile *vm = fileSystem->Open("VirtualMemory");
    stats->numDiskReads++;
    vm->ReadAt(&(machine->mainMemory[pageTable[newPage].physicalPage * PageSize]), PageSize,
               pageTable[newPage].virtualPage * PageSize);
    delete vm;
}
```

#### PC的细节

中断操作中我们使用了如下代码：

```cpp
void Interrupt::PageFault(int BadVAddr){
    printf("badVAddr is %d\n",badVAddr);
    currentThread->space->FIFO(badVAddr);
    stats->numPageFaults++;
    machine->registers[NextPCReg]=machine->registers[PCReg];
    machine->registers[PCReg]-=4;
    printf("PCReg=%d,NextPCReg=%d\n",machine->registers[PCReg],machine->registers[NextPCReg]);
}
```

其中我们对PC寄存器的修改貌似与其他的中断都不一样，这是有原因的，回忆我们课上讲过的知识，操作系统（内核态）会按如下流程处理页错误：

> - 检查进程的内部页表（进程维护的内部页表和 PCB 一起保存）来确定该引用是否为合法地址，若引用非法则终止进程，否则准备从磁盘中调入页面；
> - 操作系统寻找一个空闲帧（如从空闲帧链表中选择一个元素）；
> - 操作系统调度 磁盘，将需要的页调入到上一步分配的空闲帧；
> - 磁盘读操作完成后修改进程的内部页表和操作系统维护的页表以表示该页已经位于内存中（重置页表、填入帧号及修改页表标志位）；
> - 重新读取因为页错误而中断的指令，进程可以访问此前无法访问的页。

这里的PC目的就是重新读取因为页错误而中断的指令。

#### 其他代码

```cpp
// exception.cc 
#include "copyright.h"
#include "system.h"
#include "syscall.h"
void StartProcess(int args);
void AdvancePC();

void
StartProcess(int *args)
{
    char *filename=(char *)args;
    OpenFile *executable = fileSystem->Open(filename);
    AddrSpace *space;

    if (executable == NULL) {
        printf("Unable to open file %s\n", filename);
        return;
    }
    space = new AddrSpace(executable);
    currentThread->space = space;

    delete executable;       // close file

    space->InitRegisters();       // set the initial register values
    space->RestoreState();    // load page table register

    machine->Run();          // jump to the user progam
    ASSERT(FALSE);       // machine->Run never returns;
    // the address space exits
    // by doing the syscall "exit"
}

void ExceptionHandler(ExceptionType which){
    int type = machine->ReadRegister(2);

    if ((which == SyscallException) && (type == SC_Halt)) {
        DEBUG('a', "Shutdown, initiated by user program.\n");
        interrupt->Halt();
    }else if((which==SyscallException)&&(type == SC_Exec)){
        interrupt->Exec();
        AdvancePC();
    }else if((which==SyscallException)&&(type==SC_Exit)){
        int num=machine->ReadRegister(4);
        printf("exit!\n");
        AdvancePC();
        currentThread->Finish();
    }else if ((which == SyscallException) && (type == SC_PrintInt)) {
        int val = machine->ReadRegister(4); //将MIPS machine存的参数取出来
        interrupt->PrintInt(val);  //执行内核的system call
        AdvancePC();
    }
    else if(which==PageFaultException){
        int badVAddr=(int)machine->ReadRegister(BadVAddrReg);
        interrupt->PageFault(badVAddr);
    } else {
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(FALSE);
    }
}

void AdvancePC(){
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(PCReg) + 4);
    machine->WriteRegister(NextPCReg, machine->ReadRegister(PCReg)+4);
}
```

&nbsp;

### 实验结果

运行shell将结果输出到`temp.log`：

<img src="OS课设\27.png" style="zoom:80%;" />

<img src="OS课设\26.png" style="zoom:80%;" />

&nbsp;

### 开放问题的回答

> 使用lab7目录中的示例程序n7(若lab7额外实现了多种算法，可用自己的lab7)，测试用户程序用同样ARRAYSIZE参数值的sort，但不同的页置换算法(详见code/lab7/n7readme.txt)多次运行n7。不同页置换算法运行结束时显示的user ticks数是否一样？解释这是为什么？
>

运行不同页置换算法并输出到文件：

```sh
./n7 -pra -1 -x ../test/sort.noff 2>&1 | tee FIFO.log
./n7 -pra -2 -x ../test/sort.noff 2>&1 | tee CLOCK.log
./n7 -pra -3 -x ../test/sort.noff 2>&1 | tee ECLOCK.log
./n7 -pra -4 -x ../test/sort.noff 2>&1 | tee LRU.log
./n7 -pra -5 -x ../test/sort.noff 2>&1 | tee RANDOM.log
```

<img src="OS课设\28.png" style="zoom:80%;" />

其中：

FIFO：

<img src="OS课设\29.png" style="zoom:80%;" />



LRU：

<img src="OS课设\30.png" style="zoom:80%;" />

CLOCK：

<img src="OS课设\31.png" style="zoom:80%;" />

EHANCED-CLOCK:

<img src="OS课设\32.png" style="zoom:80%;" />

RANDOM:

<img src="OS课设\33.png" style="zoom:80%;" />

Ticks相当于对于系统的模拟时钟的一 "跳" 的模拟，在`stats.h`中我们可以找到相应的定义：

<img src="OS课设\34.png" style="zoom:80%;" />

其中`totalTicks`等，或者说，我们最关心的`userTicks`的变更，发生在`interrupt.cc`的`OneTick()`方法中，该方法将时钟提前一个刻度，并为任何待处理的请求提供服务（通过调用`CheckIfDue()`）。它在每个用户级指令执行后在`machine::Run()`调用，以及在中断恢复时由`SetLevel()`调用。

Nachos通过维护事件队列和模拟时钟来模拟中断。随着时钟滴答，检查事件队列以查找中计划现在发生的事件。时钟完全由软件维护，并在下列条件下滴答:

1. 每次恢复中断（并且恢复的中断掩码已启用中断）时，时钟前进一个刻度。Nachos代码经常通过显式调用`interrupt::SetLevel()`来禁用和恢复中断互斥锁。

   - 不过只有两种`Level`，开与关

2. 每当MIPS模拟器执行一条指令时，时钟前进一个刻度。

3. 每当准备列表为空时，时钟就会前进所需要的数量的节拍，来快进当前时间到下一个预定事件的时间。

每当时钟前进时，都会检查事件队列，并通过调用与计时器事件相关联的过程（例如中断服务例程）来服务任何挂起的中断事件。所有中断服务routine都在禁用中断的情况下运行，中断服务routine可能不会重新启用它们。

而**由于我们不同算法中的缺页次数并不相同，所以`userTicks`并不相同，其实我们可以发现，将`userTicks`减去缺页次数，我们就很容易发现所得的`Ticks`结果就是一样的**。

> 最优页置换算法(OPT)有最低的页故障率，但需要未来的页面引用信息，因此不能用于实际环境，主要用于评估其他页置换算法的性能。在前述1-5实现的基础上，给出在Nachos中获得最优页置换算法页故障次数的具体实现方法(不要求实现可运行的代码。在实验报告中用文字描述即可，必要时可在文字中结合进关键代码片段、数据结构、对象等说明)。
>

最优置换（optimal page-replacement） 是所有页置换算法中页错误率最低的，但它需要引用串的先验知识，因此无法被实现。它会将内存中的页 P 置换掉，页 P 满足：从现在开始到未来某刻再次需要页 P，这段时间最长。也就是 OPT 算法会 置换掉未来最久不被使用的页 。

如下例，初始时通过页错误调入7,0,1，然后加载页号为2的页时没找到，在未来最久不会被使用（也就是在引用串中从当前位置往后找，找页号中在引用串中距离当前位置最远的页号）的页号为7，所以将7替换成2。

![](https://zwn2001.github.io/2022/06/28/os-note7-13/37.png)

我们使用如下的类：

<img src="OS课设\35.png" style="zoom:120%;" />

我们首先给程序分配足够大的内存，获得程序运行的引用串，关于如何获得引用串：程序每次访存都会通过`translate.cc`中的`translate()`方法进行地址转换，我们只需要在此时记录下所引用的页序号即可，并将其保存在数组中，在程序执行完后，退出前将引用串写入特定文件。

然后我们再次运行原程序，使用给定的页数，如5页，我们首先从文件中获取上次运行的引用串，基于此引用串使用OPT进行置换，每次发生页错误时`numPageFaults`就会自增，从而完成记录页错误次数。

&nbsp;

## 引用

### VMware虚拟机网络配置-NAT篇

**想要达成的效果**

主机和虚拟机之间，可以互相ping通，若主机可以上网，则虚拟机亦可以正常连接网络。

**注：**与当前主机处于同一个局域网的其他主机是不能访问当前主机上的虚拟机的。

#### **准备工作**

##### **开启ping回应**

win10默认是不开启ping回应的，要设置为开启状态。

步骤：控制面板-Windows Defender防火墙-左侧的 高级设置-入站规则，在列表中搜索：

<img src="OS课设\1.png" style="zoom:70%;" />

在这两项上，右键-启用规则。

##### **主机IP**

在主机上，win+r，打开运行，输入cmd，打开命令行，输入ipconfig，显示如下：

<img src="OS课设\2.png" style="zoom:60%;" />

我主机是笔记本，连接的是无线网，因此选择无线局域网适配器，如果连接的是有线网，就选择以太网适配器。

记住主机IP地址，以我为例，是 192.168.31.134。

##### **安装VMware，安装操作系统**

网上有现成的安装教程，自己搜索就是，也可以参照我自己写的：

[VMware虚拟机安装教程zhanglonglong12.gitee.io/posts/3128323722/](https://link.zhihu.com/?target=https%3A//zhanglonglong12.gitee.io/posts/3128323722/)

##### **原理**

在实现之前，得先说一下NAT的原理，从根本上理解。

<img src="OS课设\3.png" style="zoom:60%;" />

> 评论区：VMnet8虚拟交换机是没有 IP 的，虚拟 NAT 设备的 IP 才是 192.168.80.1。不影响配置

先记住这个拓扑图，后面的设置都是按照这个图来的。

虚拟机的IP都是由虚拟交换机决定的，由虚拟NAT负责连接外网，而虚拟网卡VMware Network Adapter Vmnet8只是负责主机与虚拟机之间的通信。

##### **NAT网络配置**

打开VMware Workstation Pro，选择虚拟机的网络连接类型，新建虚拟机，一般默认就是NAT模式，之后确定，返回主页面：

<img src="OS课设\4.png" style="zoom:50%;" />

选择编辑-虚拟网络编辑器，单击更改设置：

<img src="OS课设\5.png" style="zoom:50%;" />


按照箭头指向，勾选对应内容：

<img src="OS课设\6.png" style="zoom:50%;" />

注意下面的子网IP，前两部分必须和主机IP的前两部分一样，即192.168，第三部分得不一样，不能是31，这里填80，第四部分补0。子网掩码都是255.255.255.0，填写完毕后，点击 NAT设置：

<img src="OS课设\7.png" style="zoom:70%;" />

填入网关IP，前三部分，即网段必须是192.168.80，和前面保持一致，第四部分随意，这里填2，网关IP整体就是192.168.80.2，**图1中的虚拟交换机IP**就是这里的网关IP，点击确定后返回，再点击 DHCP设置：

<img src="OS课设\8.png" style="zoom:70%;" />

注意起始和结束的IP网段也必须是192.168.80，这个范围不要包括网关IP：192.168.80.2，**图1中的虚拟DHCP服务器**设置完成，之后一路确定，NAT网络设置结束。（如果NAT模式下，有很多台虚拟机，就把这个范围扩大）

**6 测试**

开启虚拟机，Ubuntu默认的IP设置是自动获取，不需要更改，当然设置静态IP也是可以的，参照图1即可。此时Ubuntu是可以正常连接外网的。

接下来测试图1中的虚拟网卡作用，在主机上，win+q，打开搜索框，输入 网络连接，回车，打开网络连接面板：

<img src="OS课设\9.png" style="zoom:50%;" />

这里的VMware Network Adapter Vmnet8就是**图1中的虚拟网卡**，另外三个分别是主机的无线网卡、主机的有线网卡、VMware仅主机模式的虚拟网卡。右击Vmnet8，选择属性：

<img src="OS课设\10.png" style="zoom:70%;" />

选择IPV4，点击属性：

<img src="OS课设\11.png" style="zoom:60%;" />

可以选择自动获取，也可以手动设置，注意，手动的话，IP不能和虚拟网关IP一样，即不能是192.168.80.2，之后一路确定，设置完成。

在Ubuntu终端和win10命令行中，互相ping各自的IP地址，然后在虚拟网卡Vmnet8，右键-禁用，在互相ping各自的IP地址，体会前后的区别。

##### **总结**

自己动手设置几次，配合图1的拓扑结构图，真正的理解了。

VMware Network Adapter Vmnet8的作用是联通主机与虚拟机，有心的话，当在主机上ssh虚拟机时，会发现虚拟机上显示的IP来源（W命令），就是VMware Network Adapter Vmnet8的地址，而不是主机上的（有线网卡、无线网卡）地址。

##### **问题**

遇到的其余问题：

- 有的教程提到，需要把主机的正在使用的网卡，与VMnet8共享，经过测试，在NAT模式下，这是多余的。

- 有时候，本来可以互相ping通的，突然间主机ping不通虚拟机了，而虚拟机可以正常联网，可以ping主机，这时候，问题就出在 VMware Network Adapter Vmnet8了，默认是自动获取IP地址，右键禁用后，再启用，一般就可以解决问题。如果还不行的话，就设置 VMware Network Adapter Vmnet8的静态IP，与虚拟机同网段，问题就解决了。

注1：只要虚拟机可以正常上外网，VMware和虚拟机的网络设置就没有问题。

注2：

在实际情况中，第二个问题**极其常见**，经常是虚拟机一段时间不用了，再打开，主机就ping不通虚拟机，也ssh连接不上，这种情况99%都是VMware Network Adapter Vmnet8的IP和虚拟机IP不在同一个网段导致的；

VMware Network Adapter Vmnet8的IP如果没有设置静态IP的话，其IP地址随着主机开关机是会变化的。当虚拟机启动后，禁用，然后在启用，就会自动设置同网段IP了。

注3：

如果设置VMware Network Adapter Vmnet8的静态IP，注意一下网段，要是和主机网卡的IP段一样，比如无线，会导致网络连接不上。（本人教训啊，手机连接wifi没有问题，笔记本就是连不上，折腾了好长时间，才想起来设置的静态IP，冲突）

##### **参考**

> [https://blog.csdn.net/w_j_r/article/details/81290995](https://link.zhihu.com/?target=https%3A//blog.csdn.net/w_j_r/article/details/81290995)
> [https://www.cnblogs.com/linjiaxin/p/6476480.html](https://link.zhihu.com/?target=https%3A//www.cnblogs.com/linjiaxin/p/6476480.html)

##### **更新：如何设置静态IP？**

Ubuntu Server下，18.04和20.04：

```text
sudo vi /etc/netplan/00-installer-config.yaml

# 原来的是自动获取IP的，可以备份一下，不备份也可以

# 清空数据
:%d

# 写入下面内容
network:
  version: 2
  renderer: networkd
  ethernets:
    ens33:   #配置的网卡名称，自己根据情况修改
      dhcp4: no    #dhcp4关闭
      dhcp6: no    #dhcp6关闭
      addresses: [192.168.80.6/24]   #设置本机IP及掩码，注意网段，80，必须和前面一样
      gateway4: 192.168.80.2   #设置网关，这个实际上就是那个虚拟交换机的
      nameservers:
          addresses: [192.168.80.2]   #设置DNS，和gateway4设置的一样即可，不需要设为8.8.8.8

# 保存退出
:wq

# 启用
sudo netplan apply

# 看一下效果
ifconfig
ping 1.cn
```

Ubuntu桌面版的静态IP设置，直接在网络设置那块改就行了，子网掩码是255.255.255.0，和ip/24的效果是一样的。

### Another

来自北大软微的笔记 https://github.com/notfresh/os_note_all/blob/fb90d0e635e58b3536174528827156624335f066/Lab/Lab4_VirtualMemory/README.md

# Lab 4: Virtual Memory (and User Program)

> **Hints for this Lab**
>
> 1. Character sequencial order
>    * VMware virtual machine and normal PC is using [Little-Endian](https://en.wikipedia.org/wiki/Endianness#Little-endian)
>    * In Nachos simulator processor is using [Big-Endian](https://en.wikipedia.org/wiki/Endianness#Big-endian)
>    * Be careful when using `WordToMachine()` and `ShortToMachine()` for proper transition
> 2. How program is put in address space
>    * Nachos described this in `bin/noff.h`
>      * `Struct segment` represents a segmet of a program
>      * `Struct noffHeader` define the *code section*, *initialized data section* and *uninitialized data section*
> 3. Related source code
>    * `code/machine/machine.h`
>    * `code/machine/machine.cc`
>    * `code/machine/translate.h`
>    * `code/machine/translate.cc`
>    * `code/userprog/addrspace.h`
>    * `code/userprog/addrspace.cc`
>    * `code/userprog/exception.cc`

There is a micro called `HOST_IS_BIG_ENDIAN`. The `CheckEndian()` in `code/machine/machine.cc` will check the format by using a array.

The exception supported by Nachos is defined in `code/machine/machine.cc` and `code/machine/machine.h` Note that page fault and no TLB entry shared `PageFaultException` depends on using linear page table or TLB.

```cpp
// Textual names of the exceptions that can be generated by user program
// execution, for debugging.
static char* exceptionNames[] = { "no exception", "syscall", 
				"page fault/no TLB entry", "page read only",
				"bus error", "address error", "overflow",
				"illegal instruction" };
```

```cpp
enum ExceptionType { NoException,           // Everything ok!
		     SyscallException,      // A program executed a system call.
		     PageFaultException,    // No valid translation found
		     ReadOnlyException,     // Write attempted to page marked 
					    // "read-only"
		     BusErrorException,     // Translation resulted in an 
					    // invalid physical address
		     AddressErrorException, // Unaligned reference or one that
					    // was beyond the end of the
					    // address space
		     OverflowException,     // Integer overflow in add or sub.
		     IllegalInstrException, // Unimplemented or reserved instr.
		     
		     NumExceptionTypes
};
```

The register is defined in `code/machine/machine.h`

> Nachos implement all the thirty-two MIPS R2/3000 Registers.
> Additionally, with 8 more special purpose register for better simulation and debug usage.

```cpp
// User program CPU state.  The full set of MIPS registers, plus a few
// more because we need to be able to start/stop a user program between
// any two instructions (thus we need to keep track of things like load
// delay slots, etc.)

// MIPS GPRs
#define StackReg	29	// User's stack pointer
#define RetAddrReg	31	// Holds return address for procedure calls

#define NumGPRegs	32	// 32 general purpose registers on MIPS

// Nachos SPRs
#define HiReg		32	// Double register to hold multiply result
#define LoReg		33
#define PCReg		34	// Current program counter
#define NextPCReg	35	// Next program counter (for branch delay) 
#define PrevPCReg	36	// Previous program counter (for debugging)
#define LoadReg		37	// The register target of a delayed load.
#define LoadValueReg 	38	// The value to be loaded by a delayed load.
#define BadVAddrReg	39	// The failing virtual address on an exception

#define NumTotalRegs 	40
```

[The MIPS registers](http://www.cs.uwm.edu/classes/cs315/Bacon/Lecture/HTML/ch05s03.html)

| Register Number         | Conventional Name                                 | Usage                                                  |
| ----------------------- | ------------------------------------------------- | ------------------------------------------------------ |
| $0              | $zero | Hard-wired to 0                                   |                                                        |
| $1              | $at   | Reserved for pseudo-instructions                  |                                                        |
| 2−3                     | v0, v1                                            | Return values from functions                           |
| 4−7                     | a0−a3                                             | Arguments to functions - not preserved by subprograms  |
| 8−15                    | t0−t7                                             | Temporary data, not preserved by subprograms           |
| 16−23                   | s0−s7                                             | Saved registers, preserved by subprograms              |
| 24−25                   | t8−t9                                             | More temporary registers, not preserved by subprograms |
| 26−27                   | k0−k1                                             | Reserved for kernel. Do not use.                       |
| $28             | $gp   | Global Area Pointer (base of global data segment) |                                                        |
| $29             | $sp   | Stack Pointer                                     |                                                        |
| $30             | $fp   | Frame Pointer                                     |                                                        |
| $31             | $ra   | Return Address                                    |                                                        |

When define `USER_PROGRAM` (Compile `code/userprog`) the Machine will be create in `code/threads/system.h`

```cpp
#ifdef USER_PROGRAM
#include "machine.h"
extern Machine* machine;	// user program memory and registers
#endif
```

And in `code/threads/system.cc`

```cpp
void
Initialize(int argc, char **argv)
{
    // ...

#ifdef USER_PROGRAM
    bool debugUserProg = FALSE;	// single step user program
#endif

    // ...

#ifdef USER_PROGRAM
    if (!strcmp(*argv, "-s"))
        debugUserProg = TRUE;
#endif

    // ...

#ifdef USER_PROGRAM
    machine = new Machine(debugUserProg);	// this must come first
#endif
}
```

> Debug using `m` for machine emulation (including exception) and `a` for address spaces.
> Use `-s` to debug user program in single step. (this will show machine registers including PC and assembly instructions)

## I. TLB exception handling

> Currently, Nachos memory management is using software simulate the TLB mechanism.
> Its working principle, exception handling, replacement algorithm is similar with paging memory management.

### Exercise 1: Trace code

> Read the `code/userprog/progtest.cc`, understand the procedure of Nachos executing user program and memory menagement related point
>
> Read the following code, understand current Nachos TLB technique and Address Binding (地址轉換) mechanism
>
> * `code/machine/machine.h`
> * `code/machine/machine.cc`
> * `code/userprog/exception.cc`

#### Executing User Program

The `code/userprog/progtest.cc` is used to put all the test to simulate a user space program.

The entry point is in `code/threads/main.cc`. When using `-x` to execute Nachos, it will call `StartProcess()`, and it is defined in `code/userprog/progtest.cc`

In `StartProcess()` it will do three main step.

1. Open the executable file
   * using `FileSystem::Open()` which is defined in `code/filesys/filesys.cc`
2. Create a address space for the executable. Assign to current thread and then initial the register.

    ```cpp
    space = new AddrSpace(executable);
    currentThread->space = space;

    space->InitRegisters();		// set the initial register values
    space->RestoreState();		// load page table register
    ```

3. Finally, call `Machine::Run()` to run the user program. And it's defined in `code/machine/mipssim.cc`

#### TLB Technique and Address Binding

In `code/machine/machine.cc` there is a macro called `USE_TLB` that controls whether to use TLB. (which should be enabled while compiling. I'll mention in [Exercise 2](#Exercise-2:-TLB-MISS-exception-handling))

The main memory is also defined in `code/machine/machine.cc` as `mainMemory` as an array of `char` with the size of `MemorySize`.

The initial TLB class `TranslationEntry` is declared in `code/machine/translate.h`. The TLB object has been used in `code/userprog/addrspace.h` as `*pageTable` and in `code/machine/machine.h` as `*tlb` (read-only pointer) and `*pageTable`. => Shared with Pate Table and TLB!

As mention, machine has a pointer to the "page table of the current user process" and a pointer to "system TLB".

The **memory translation** is defined in `code/machine/translate.cc` as `Machine::Translate()` (but is declared in `code/machine/machine.h`). The procedure is as following.

1. First get `vpn` and `offset`

    ```cpp
    vpn = (unsigned) virtAddr / PageSize;
    offset = (unsigned) virtAddr % PageSize;
    ```

2. There are two cases: use linear page table or use TLB (not both at the same time "in original implementation assumption")
   * If use page table

        ```cpp
        if (vpn >= pageTableSize) {
            DEBUG('a', "virtual page # %d too large for page table size %d!\n", virtAddr, pageTableSize);
            return AddressErrorException;
        } else if (!pageTable[vpn].valid) {
            DEBUG('a', "virtual page # %d too large for page table size %d!\n", virtAddr, pageTableSize);
            return PageFaultException;
        }

        entry = &pageTable[vpn];
        ```

   * If use TLB

        ```cpp
        for (entry = NULL, i = 0; i < TLBSize; i++)
            if (tlb[i].valid && (tlb[i].virtualPage == vpn)) {
            entry = &tlb[i];			// FOUND!
            break;
            }
        
        if (entry == NULL) {				// not found
                DEBUG('a', "*** no valid TLB entry found for this virtual page!\n");
                return PageFaultException;		// really, this is a TLB fault,
                            // the page may be in memory,
                            // but not in the TLB
        }
        ```

3. Read-only error detection
4. Get the page frame number

    ```cpp
    pageFrame = entry->physicalPage
    ```

5. Check if the page frame is valid
6. Combine to get the physical address!

    ```cpp
    *physAddr = pageFrame * PageSize + offset
    ```

The `Machine::Translate()` is called when using `Machine::ReadMem()` and `Machine::WriteMem()`. When the exception is raised it will called `Machine::RaiseException()`. In it will switch to System Mode and call the `ExceptionHandler()` in `code/userprog/exception.cc`. And then it will call `Machine::ReadRegister(2)` to get the type. But in initial state, this only handle the `SC_Halt` type `SyscallException`. (The system call types are defined in `code/userprog/syscall.h`)

#### Memory Related Definition

In `code/machine/machine.h`.

```cpp
// Definitions related to the size, and format of user memory

#define PageSize 	SectorSize 	// set the page size equal to
					// the disk sector size, for
					// simplicity

#define NumPhysPages    32
#define MemorySize 	(NumPhysPages * PageSize)
#define TLBSize		4		// if there is a TLB, make it small
```

In `code/machine/disk.h`.

```cpp
#define SectorSize 		128	// number of bytes per disk sector
```

So we can found that the total memory size `MemorySize` is **32 × 128 Bytes = 4KB**.

### Exercise 2: TLB MISS exception handling

> Modify the `ExceptionHandler()` in `code/userprog/exception.cc`. Makes Nachos able to handle TLB exception
>
> (When TLB exception, Nachos will throw **PageFaultException**. Reference `code/machine/machine.cc`)

To use TLB must define the `USE_TLB` macro that I've mentioned in Exercise 1.

So add `-DUSE_TLB` in `code/userprog/Makefile`

```makefile
DEFINES = -DUSE_TLB
```

> I was decided to change the original `ExceptionHandler()` in `code/userprog/exception.cc` to use switch case instead of if else.
> But I want to make as little modification as possible at this point.
> So maybe wait until implementing system call.

#### How Nachos Fetch Instruction

In `code/machine/mipssim.cc`

```c
Machine::Run() {
    ...

    while (1) {
        OneInstruction(instr);

        ...
    }
}
```

```c
Machine::OneInstruction(Instruction *instr)
{
    ...

    // Fetch instruction
    if (!machine->ReadMem(registers[PCReg], 4, &raw))
        return;			// exception occurred
    instr->value = raw;
    instr->Decode();

    ...

    // Compute next pc, but don't install in case there's an error or branch.
    int pcAfter = registers[NextPCReg] + 4;

    ...


    // Now we have successfully executed the instruction.

    ...

    // Advance program counters.
    registers[PrevPCReg] = registers[PCReg];	// for debugging, in case we
						// are jumping into lala-land
    registers[PCReg] = registers[NextPCReg];
    registers[NextPCReg] = pcAfter;
```

If Nachos fail fetch instruction (most likely to be Page Fault Exception). Then it won't execute PC+4 because `machine->ReadMem()` will return false.

Thus we only need to update TLB in exception handler, and Nachos will execute same instruction and try to translate again.

#### The BadVAddr Register in MIPS

This register (its name stands for **Bad V**irtual **Addr**ess) will contain the memory address where the exception
has occurred. An unaligned memory access, for instance, will generate an exception and the address where
the access was attempted will be stored in BadVAddr.

#### Page Fault

There are two cases will lead to page fault.

1. TLB miss
   * When doing either `Machine::ReadMem()` or `Machine::WriteMem()` in `code/machine/translate.cc` and TLB fail. It will pass `addr` and call `Machine::RaiseException()` in `code/machine/machine.cc`. And then it will preserve this address in `BadVAddrReg`

        ```cpp
        registers[BadVAddrReg] = badVAddr;
        ```

   * Thus all we need to do is to read the address from the `BadVAddrReg` and then calculate the `vpn` in `ExceptionHandler()` in `code/userprog/exception.cc`. And there are two cases
     1. There is empty entry in TLB. Insert it into TLB.
     2. TLB is full. Replace with some algorithm which will be implement in [Exercise 3](#Exercise-3:-Replacement-algorithm).
2. Page table failed
   * For now this won't happen because all the page frame are loaded in the memory.
     * When `new AddrSpace(executable)` in `code/userprog/progtest.cc` the `StartProcess()` function. While initializing a address space will check `ASSERT(numPages <= NumPhysPages)`. (Thus the total user program is [limited in 4KB](#Memory-Related-Definition)) => So page table won't fail at this point.
     * The problem will be mentioned again and will be solved in [Exercise 7](#Exercise-7:-Loading-page-on-demand) which will implement the demend paging technique.

Disable the `ASSERT(tlb == NULL || pageTable == NULL);` in `code/machine/translate.cc` to let TLB exist with the page table.

The way to seperate the exception caused by TLB or origianl linear page table is to check `if(machine->tlb == NULL)`. Just like the default Nachos did in the `Machine::Translate` in `code/machine/translate.cc`.

In `code/userprog/exception.cc` append the `ExceptionHandler()` as the following:

```cpp
void
ExceptionHandler(ExceptionType which)
{
    // Lab4: Page Fault Handling
    if (which == PageFaultException) {
        if (machine->tlb == NULL) { // linear page table page fault
            DEBUG('m', "=> Page table page fault.\n");
            // In current Nachos this shouldn't happen
            // because physical page frame == virtual page number
            // (can be found in AddrSpace::AddrSpace in userprog/addrspace.cc)
            // On the other hand, in our Lab we won't use linear page table at all
            ASSERT(FALSE);
        } else { // TLB miss (no TLB entry)
            // Lab4 Exercise2
            DEBUG('m', "=> TLB miss (no TLB entry)\n");
            int BadVAddr = machine->ReadRegister(BadVAddrReg); // The failing virtual address on an exception
            TLBMissHandler(BadVAddr);
        }
        return;
    }

    // System Call
    ...
}
```

#### Test Exercise 2

In this exercise, I test the TLB with only 2 entry to simplify the test.
There is a reason that we couldn't simplify test with TLB with only 1 entry. Because in `Machine::OneInstruction()` it will always need to fetch the instruction using `Machine::ReadMem()`.
But in some operation it will need to use `Machine::WriteMem()`. If we fail on the second `Machine::Translate()`, it will return False. And re-fetch the instruction again. And thus override the TLB miss handling that we've just made for the second one.
This will end up cause the infinity loop.

Here is the simplified `TLBMissHandler()`

```cpp
int TLBreplaceIdx = 0;

void
TLBMissHandler(int virtAddr)
{
    unsigned int vpn;
    vpn = (unsigned) virtAddr / PageSize;

    // ONLY USE FOR TESTING Lab4 Exercise2
    // i.e. assume TLBSize = 2
    machine->tlb[TLBreplaceIdx] = machine->pageTable[vpn];
    TLBreplaceIdx = TLBreplaceIdx ? 0 : 1;
}
```

> Test using the executable test program in `code/test`.
> (If the file is too big will get `Assertion failed: line 81, file "../userprog/addrspace.cc"`)
>
> Using docker (build with `cd Lab/Lab0_BuildNachos; ./build_subdir_nachos.sh userprog`)
>
> ```sh
> docker run nachos_userprog nachos/nachos-3.4/code/userprog/nachos -d am -x nachos/nachos-3.4/code/test/halt

Here is the result:

```txt
Initializing address space, num pages 10, size 1280
Initializing code segment, at 0x0, size 256
Initializing stack register to 1264
Starting thread "main" at time 10
Reading VA 0x0, size 4
        Translate 0x0, read: *** no valid TLB entry found for this virtual page!
Exception: page fault/no TLB entry
=> TLB miss (no TLB entry)
Reading VA 0x0, size 4
        Translate 0x0, read: phys addr = 0x0
        value read = 0c000034
At PC = 0x0: JAL 52
Reading VA 0x4, size 4
        Translate 0x4, read: phys addr = 0x4
        value read = 00000000
At PC = 0x4: SLL r0,r0,0
Reading VA 0xd0, size 4
        Translate 0xd0, read: *** no valid TLB entry found for this virtual page!
Exception: page fault/no TLB entry
=> TLB miss (no TLB entry)
Reading VA 0xd0, size 4
        Translate 0xd0, read: phys addr = 0xd0
        value read = 27bdffe8
At PC = 0xd0: ADDIU r29,r29,-24
Reading VA 0xd4, size 4
        Translate 0xd4, read: phys addr = 0xd4
        value read = afbf0014
At PC = 0xd4: SW r31,20(r29)
Writing VA 0x4ec, size 4, value 0x8
        Translate 0x4ec, write: *** no valid TLB entry found for this virtual page!
Exception: page fault/no TLB entry
=> TLB miss (no TLB entry)
Reading VA 0xd4, size 4
        Translate 0xd4, read: phys addr = 0xd4
        value read = afbf0014
At PC = 0xd4: SW r31,20(r29)
Writing VA 0x4ec, size 4, value 0x8
        Translate 0x4ec, write: phys addr = 0x4ec
Reading VA 0xd8, size 4
        Translate 0xd8, read: phys addr = 0xd8
        value read = afbe0010
At PC = 0xd8: SW r30,16(r29)
Writing VA 0x4e8, size 4, value 0x0
        Translate 0x4e8, write: phys addr = 0x4e8
Reading VA 0xdc, size 4
        Translate 0xdc, read: phys addr = 0xdc
        value read = 0c000030
At PC = 0xdc: JAL 48
Reading VA 0xe0, size 4
        Translate 0xe0, read: phys addr = 0xe0
        value read = 03a0f021
At PC = 0xe0: ADDU r30,r29,r0
Reading VA 0xc0, size 4
        Translate 0xc0, read: phys addr = 0xc0
        value read = 03e00008
At PC = 0xc0: JR r0,r31
Reading VA 0xc4, size 4
        Translate 0xc4, read: phys addr = 0xc4
        value read = 00000000
At PC = 0xc4: SLL r0,r0,0
Reading VA 0xe4, size 4
        Translate 0xe4, read: phys addr = 0xe4
        value read = 0c000004
At PC = 0xe4: JAL 4
Reading VA 0xe8, size 4
        Translate 0xe8, read: phys addr = 0xe8
        value read = 00000000
At PC = 0xe8: SLL r0,r0,0
Reading VA 0x10, size 4
        Translate 0x10, read: *** no valid TLB entry found for this virtual page!
Exception: page fault/no TLB entry
=> TLB miss (no TLB entry)
Reading VA 0x10, size 4
        Translate 0x10, read: phys addr = 0x10
        value read = 24020000
At PC = 0x10: ADDIU r2,r0,0
Reading VA 0x14, size 4
        Translate 0x14, read: phys addr = 0x14
        value read = 0000000c
At PC = 0x14: SYSCALL
Exception: syscall
Shutdown, initiated by user program.
Machine halting!
```

And it shows that the TLB mechanism worked!

> If use original linear page table
>
> ```txt
> Initializing address space, num pages 10, size 1280
> Initializing code segment, at 0x0, size 256
> Initializing stack register to 1264
> Starting thread "main" at time 10
> Reading VA 0x0, size 4
>      Translate 0x0, read: phys addr = 0x0
>      value read = 0c000034
> At PC = 0x0: JAL 52
> Reading VA 0x4, size 4
>      Translate 0x4, read: phys addr = 0x4
>      value read = 00000000
> At PC = 0x4: SLL r0,r0,0
> Reading VA 0xd0, size 4
>      Translate 0xd0, read: phys addr = 0xd0
>      value read = 27bdffe8
> At PC = 0xd0: ADDIU r29,r29,-24
> Reading VA 0xd4, size 4
>      Translate 0xd4, read: phys addr = 0xd4
>      value read = afbf0014
> At PC = 0xd4: SW r31,20(r29)
> Writing VA 0x4ec, size 4, value 0x8
>      Translate 0x4ec, write: phys addr = 0x4ec
> Reading VA 0xd8, size 4
>      Translate 0xd8, read: phys addr = 0xd8
>      value read = afbe0010
> At PC = 0xd8: SW r30,16(r29)
> Writing VA 0x4e8, size 4, value 0x0
>      Translate 0x4e8, write: phys addr = 0x4e8
> Reading VA 0xdc, size 4
>      Translate 0xdc, read: phys addr = 0xdc
>      value read = 0c000030
> At PC = 0xdc: JAL 48
> Reading VA 0xe0, size 4
>      Translate 0xe0, read: phys addr = 0xe0
>      value read = 03a0f021
> At PC = 0xe0: ADDU r30,r29,r0
> Reading VA 0xc0, size 4
>      Translate 0xc0, read: phys addr = 0xc0
>      value read = 03e00008
> At PC = 0xc0: JR r0,r31
> Reading VA 0xc4, size 4
>      Translate 0xc4, read: phys addr = 0xc4
>      value read = 00000000
> At PC = 0xc4: SLL r0,r0,0
> Reading VA 0xe4, size 4
>      Translate 0xe4, read: phys addr = 0xe4
>      value read = 0c000004
> At PC = 0xe4: JAL 4
> Reading VA 0xe8, size 4
>      Translate 0xe8, read: phys addr = 0xe8
>      value read = 00000000
> At PC = 0xe8: SLL r0,r0,0
> Reading VA 0x10, size 4
>      Translate 0x10, read: phys addr = 0x10
>      value read = 24020000
> At PC = 0x10: ADDIU r2,r0,0
> Reading VA 0x14, size 4
>      Translate 0x14, read: phys addr = 0x14
>      value read = 0000000c
> At PC = 0x14: SYSCALL
> Exception: syscall
> Shutdown, initiated by user program.
> Machine halting!
> ```

### Exercise 3: Replacement algorithm

> Implement at least two replacement algorithm, compare the replacement times between two algorithm.

There are preparations before test the algorithms

1. TLB Miss Rate

    To show the TLB Miss Rate. I've declare `TLBMissCount` and `TranslateCount` in `code/machine/machine.h` and initialize in `code/machine/translate.cc`.

    * When call the `Machine::Translate()` the `TranslateCount++`.
    * When raise exception in `Machine::ReadMem()` or `Machine::WriteMem()` the `TLBMissCount++`.

    In the end, calculate the TLB Miss Rate when system halt (in `code/machine/exception.cc`). This will do only when enable TLB.

2. Custom user program

    I have made a customized user program to test the miss rate.

    > Because `code/test/halt` is too short to observe the TLB Miss. But the other program is too large for default Nachos (I'll use them after finish demand paging in Exercise 7).

    `code/test/fibonacci.c`

    ```c
    /* fibonacci.c
    *	Simple program to test the TLB miss rate (Lab 4) that calculate the fibonacci series
    */

    #include "syscall.h"

    #define N 20

    int
    main()
    {
        int i;
        int result[N];
        result[0] = 0;
        result[1] = 1;
        for (i = 2; i < N; i++)
        {
            result[i] = result[i-1] + result[i-2];
        }
        Exit(result[N-1]);
    }
    ```

    And add the following things in `code/test/Makefile`

    ```makefile
    all: ... fibonacci

    ...

    # For Lab4: Test the TLB Miss Rate
    fibonacci.o: fibonacci.c
        $(CC) $(CFLAGS) -c fibonacci.c
    fibonacci: fibonacci.o start.o
        $(LD) $(LDFLAGS) start.o fibonacci.o -o fibonacci.coff
        ../bin/coff2noff fibonacci.coff fibonacci
    ```

    This will need to compile the whole project but I've done that and copy the binary/executable file to local.
    So it's totally fine to just compile the userprog subdirectory using my docker build script.

3. Switch for each algorithm

    I have used some define as the switch for choosing the replacement algorithm. And this will also manage some global variables, etc.

    * TLB_FIFO
    * TLB_CLOCK
    * TLB_LRU (TODO)

And then I'll test the program without other verbose information. Enable debug flag `T` to show the TLB handling message (miss rate)

```sh
docker run nachos_userprog nachos/nachos-3.4/code/userprog/nachos -d T -x nachos/nachos-3.4/code/test/fibonacci
```

> Alternatively, you can build the docker with
>
> ```sh
> cd Lab/Lab0_BuildNachos;
> ./build_modified_nachos.sh
> ```
>
> and execute it. (use `-d S` to see the return value of Exit system call (to check the calculation result))
>
> ```sh
> docker run nachos nachos/nachos-3.4/code/userprog/nachos -d T -x nachos/nachos-3.4/code/test/fibonacci
> ```

#### FIFO

```c
void
TLBAlgoFIFO(TranslationEntry page)
{
    int TLBreplaceIdx = -1;
    // Find the empty entry
    for (int i = 0; i < TLBSize; i++) {
        if (machine->tlb[i].valid == FALSE) {
            TLBreplaceIdx = i;
            break;
        }
    }
    // If full then move everything forward and remove the last one
    if (TLBreplaceIdx == -1) {
        TLBreplaceIdx = TLBSize - 1;
        for (int i = 0; i < TLBSize - 1; i++) {
            machine->tlb[i] = machine->tlb[i+1];
        }
    }
    // Update TLB
    machine->tlb[TLBreplaceIdx] = page;
}
```

Result:

TLBSize = 2

```txt
TLB Miss: 87, TLB Hit: 764, Total Instruction: 851, Total Translate: 938, TLB Miss Rate: 10.22%
Machine halting!
```

Default TLBSize (= 4)

```txt
TLB Miss: 6, TLB Hit: 818, Total Instruction: 824, Total Translate: 830, TLB Miss Rate: 0.73%
```

#### Clock

Because the Nachos TLB (the `class TranslationEntry` in `code/machine/translate.h`) already has `use` (and `dirty`) bit.

We can implement the alternative second chance replacement algorithm by the clock algorithm.

> The brief description of second chance replacement algorithm:
>
> * It is designed to avoid the problem of throwing out a heavily used page in FIFO
> * Introduce R bit (i.e. `use` bit)
>   * 0: page is both old and unused, so it is replaced immediately
>   * 1: the bit is cleared, the page is put onto the end of the list of TLB (and its load time is updated as though it had just arrived in memory)

```c
int TLBreplaceIdx = 0; // When using TLB_CLOCK, this is circular pointer

void
TLBAlgoClock(TranslationEntry page)
{
    // Find the next one
    // if used then clear to 0 and continue find the next one.
    // until find the one that is not used.
    while (1) {
        TLBreplaceIdx %= TLBSize;
        if (machine->tlb[TLBreplaceIdx].valid == FALSE) {
            break;
        } else  {
            if (machine->tlb[TLBreplaceIdx].use) {
                // Found the entry is recently used
                // clear the R bit and find next
                machine->tlb[TLBreplaceIdx].use = FALSE;
                TLBreplaceIdx++;
            } else {
                // Evict the entry
                break;
            }
        }
    }

    // Update TLB
    machine->tlb[TLBreplaceIdx] = page;
    machine->tlb[TLBreplaceIdx].use = TRUE;
}
```

Result:

```txt
TLB Miss: 6, TLB Hit: 818, Total Instruction: 824, Total Translate: 830, TLB Miss Rate: 0.73%
```

#### LRU

TODO

#### Conclusion

In the fibonacci user program we can found that. Because in the scenario there is no chance to "throwing out a heavily used TLB" since the memory access is quite average.
Thus the performance of FIFO and Clock algorithm are the same.

## II. Paging Memory Management

> In the current Nachos, The member variable `AddrSpace* space` used in `Class Thread` use `TranslationEntry* pageTable` to manage memory.
> When application program is starting up it will initialize it; When context switch, it will also do reserve and resume.
> (Such that `Class Machine::TranslationEntry* pageTable` will always pointing at the current running Thread's page table)

As mention in [TLB Technique and Address Binding](#TLB-Technique-and-Address-Binding) we know that system has the pointer to current thread page table.
But we may have multiple address space (one for each thread) in the future.

We have seen in [Executing User Program](#Executing-User-Program). We will call `AddrSpace::RestoreState()` to assign the current thread page table to machine page table pointer.

### Exercise 4: Global data structure for memory management

> Impelement a global data structure for memory allocation and recycle, and record the current memory usage status
>
> e.g. Free Linked List (空閒鏈表)
>
> e.g. Bitmap(位圖)
>
> * [Memory Management with Bitmaps and Linked List](http://www.idc-online.com/technical_references/pdfs/information_technology/Memory_Management_with_Bitmaps_and_Linked_List.pdf)
>* [OS Memory Management with Bitmaps](https://codescracker.com/operating-system/memory-management-with-bitmaps.htm)
> * [OS Memory Management with Linked Lists](https://codescracker.com/operating-system/memory-management-with-linked-lists.htm)

I have used some define as the switch for choosing the data structure for memory management. And just enable in `code/userprog/Makefile`

* USE_BITMAP
* USE_LINKED_LIST (TODO)

#### Bitmap

I've divide the memory up into *allocation unit* with `NumPhysPages` and thus use `unsigned int` to store the bitmap.

Corresponding to each *allocation unit* is a bit in the bitmap:

* zero (0) only if the unit is free
* one (1) only if the unit is occupied

Add the following data structure in `class Machine` in `code/machine/machine.h`

```cpp
class Machine {
  public:
  
    ...

    unsigned int bitmap; // This can record 32 allocation units (sizeof(int)*8 = 32). Current NumPhysPages is 32 too.
    int allocateFrame(void); // Find a empty allocation unit to put physical page frames
    void freeMem(void); // Free current page table physical page frames
}
```

And implement `Machine::allocateFrame` and `Machine::freeMem` in `code/machine/machine.cc`

* `Machine::allocateFrame` is used to find an empty physical page frame to allocate

    ```cpp
    //----------------------------------------------------------------------
    // Machine::allocateFrame
    //   	Find a free physical page frame to allocate memory.
    //      If not found, return -1.
    //----------------------------------------------------------------------

    int
    Machine::allocateFrame(void)
    {
        int shift;
        for (shift = 0; shift < NumPhysPages; shift++) {
            if (!(bitmap >> shift & 0x1)) { // found empty bit
                bitmap |= 0x1 << shift; // set the bit to used
                DEBUG('M', "Allocate physical page frame: %d\n", shift);
                return shift;
            }
        }
        DEBUG('M', "Out of physical page frame!\n", shift);
        return -1;
    }
    ```

* `Machine::freeMem` is used when we want to release the page frames occupied by current page table

> TODO:
> Nachos actually has Bitmap class in `code/userprog/bitmap.cc` and `code/userprog/bitmap.h`.
> If the physical memory need to be extend (current `unsigned int` can't represent) then switch to this.

#### Free Linked List

TODO

#### Other Modification

1. We need to modify the default memory allocation from linear allocation to our own logic when initializing `AddrSpace` in `code/userprog/addrspace.cc`.
   * Bitmap

        ```c
        pageTable[i].physicalPage = machine->allocateFrame();
        ```

2. [`Exit` system call](#The-Exit-Syscall)
3. We also need to free the memory and clear the record in our data structure.
   * Bitmap

        ```c
        void AddressSpaceControlHandler(int type)
        {
            if (type == SC_Exit) {
        
                ...
        
        #ifdef USER_PROGRAM
                if (currentThread->space != NULL) {
        #ifdef USE_BITMAP
                    machine->freeMem(); // ONLY USE FOR TEST Lab4 Exercise4
        #endif
                    delete currentThread->space;
                    currentThread->space = NULL;
                }
        #endif
        
                ...
        
            }
        }
        ```

Result

> I've add `M` for memory management debug flag

```sh
docker run nachos_userprog nachos/nachos-3.4/code/userprog/nachos -d M -x nachos/nachos-3.4/code/test/halt
```

* Bitmap

    ```txt
    Allocate physical page frame: 0
    Allocate physical page frame: 1
    Allocate physical page frame: 2
    Allocate physical page frame: 3
    Allocate physical page frame: 4
    Allocate physical page frame: 5
    Allocate physical page frame: 6
    Allocate physical page frame: 7
    Allocate physical page frame: 8
    Allocate physical page frame: 9
    Bitmap after allocate: 000003FF
    Free physical page frame: 0
    Free physical page frame: 1
    Free physical page frame: 2
    Free physical page frame: 3
    Free physical page frame: 4
    Free physical page frame: 5
    Free physical page frame: 6
    Free physical page frame: 7
    Free physical page frame: 8
    Free physical page frame: 9
    Bitmap after freed: 00000000
    Machine halting!
    ```

### Exercise 5: Support multi-threads

> In the current Nachos, only single Thread can exist in memory. We need to break this restriction.

Catch up how we [execute user program](#Executing-User-Program).

And there is additional define in `code/threads/thread.h`. We can assign each thread a specific address space.

```c
class Thread {

    ...

  private:

    ...

#ifdef USER_PROGRAM // Lab4: Multi-thread user program
// A thread running a user program actually has *two* sets of CPU registers -- 
// one for its state while executing user code, one for its state 
// while executing kernel code.

    int userRegisters[NumTotalRegs];	// user-level CPU register state

  public:
    void SaveUserState();		// save user-level register state
    void RestoreUserState();		// restore user-level register state

    AddrSpace *space;			// User code this thread is running.
#endif

};
```

And the `Thread::SaveUserState()` and `Thread::RestoreUserState()` (manipulate CPU register)

> Correspond to `AddrSpace::SaveState()` and `AddrSpace::RestoreState()` (manipulate memory (i.e. system page table and TLB))

```c
//----------------------------------------------------------------------
// Thread::SaveUserState
//	Save the CPU state of a user program on a context switch.
//
//	Note that a user program thread has *two* sets of CPU registers -- 
//	one for its state while executing user code, one for its state 
//	while executing kernel code.  This routine saves the former.
//----------------------------------------------------------------------

void
Thread::SaveUserState()
{
    for (int i = 0; i < NumTotalRegs; i++)
	userRegisters[i] = machine->ReadRegister(i);
}

//----------------------------------------------------------------------
// Thread::RestoreUserState
//	Restore the CPU state of a user program on a context switch.
//
//	Note that a user program thread has *two* sets of CPU registers -- 
//	one for its state while executing user code, one for its state 
//	while executing kernel code.  This routine restores the former.
//----------------------------------------------------------------------

void
Thread::RestoreUserState()
{
    for (int i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, userRegisters[i]);
}
```

In `/code/threads/scheduler.cc`, when occur context switch, the scheduler will invoke both `Thread::SaveUserState()`, `Thread::RestoreUserState()` and `AddrSpace::SaveState()`, `AddrSpace::RestoreState()`

```c
void
Scheduler::Run (Thread *nextThread)
{
    Thread *oldThread = currentThread;
    
#ifdef USER_PROGRAM			// ignore until running user programs 
    if (currentThread->space != NULL) {	// if this thread is a user program,
        currentThread->SaveUserState(); // save the user's CPU registers
	currentThread->space->SaveState();
    }
#endif

    ...

    // Context Switch to nextThread

    ...

#ifdef USER_PROGRAM
    if (currentThread->space != NULL) {		// if there is an address space
        currentThread->RestoreUserState();     // to restore, do it.
	currentThread->space->RestoreState();
    }
#endif
}
```

#### The Exit Syscall

As the Nachos comment said, `machine->Run()` never return, the address space exits by doing the syscall "exit".

> This will also used in Exercise 4 to *clean up the memory management data structure*
> and the other previous Exercises which used to *return the result value*.

The defnition of `Exit` in the `code/userprog/syscall.h` said that. And there are also some address space related system call.

* Exit
* Exec
* Join

```c
/* Address space control operations: Exit, Exec, and Join */

/* This user program is done (status = 0 means exited normally). */
void Exit(int status);

/* A unique identifier for an executing user program (address space) */
typedef int SpaceId;

/* Run the executable, stored in the Nachos file "name", and return the
 * address space identifier
 */
SpaceId Exec(char *name);

/* Only return once the the user program "id" has finished.  
 * Return the exit status.
 */
int Join(SpaceId id);
```

The Exit system call: user process quits with status returned.

The kernel handles an Exit system call by

1. destroying the process data structures and thread(s)
2. [reclaiming any memory assigned to the process](#Other-Modification) (i.e. The memory management data structure)
3. arranging to return the exit status value as the result of the `Join` on
   this process, if any.
   * (The `Join` related part will be completed in [Lab6](../Lab6_SystemCall/README.md#Exercise-3:-Implement-user-program-system-call), we will now show the exit status value in debug message)

> The following code is define in `code/userprog/syscall.h` and implement in `code/userprog/exception.cc`.

I've made some system call handler function for further preparation.
Each handling a type of system call.

```c
void AddressSpaceControlHandler(int type); // Exit, Exec, Join
void FileSystemHandler(int type); // Create, Open, Write, Read, Close
void UserLevelThreadsHandler(int type); // Fork, Yield
```

Because system call need to return to the next instruction (unlike the page fault exception).
Thus we need to advance/increase the PC Registers. I've made this a function too.

```c
//----------------------------------------------------------------------
// IncrementPCRegs
// 	Because when Nachos cause the exception. The PC won't increment
//  (i.e. PC+4) in Machine::OneInstruction in machine/mipssim.cc.
//  Thus, when invoking a system call, we need to advance the program
//  counter. Or it will cause the infinity loop.
//----------------------------------------------------------------------

void IncrementPCRegs(void) {
    // Debug usage
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));

    // Advance program counter
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
}
```

And here is the Exit system call.

> The TODOs is prepared for the furtuer Lab (maybe)

```c
//----------------------------------------------------------------------
// AddressSpaceControlHandler
// 	Handling address space control related system call.
//  1. Exit
//  2. Exec
//  3. Join
//----------------------------------------------------------------------

void AddressSpaceControlHandler(int type)
{
    if (type == SC_Exit) {

        PrintTLBStatus(); // TLB debug usage

        int status = machine->ReadRegister(4); // r4: first arguments to functions

        currentThread->setExitStatus(status);
        if (status == 0) {
            DEBUG('S', COLORED(GREEN, "User program exit normally. (status 0)\n"));
        } else {
            DEBUG('S', COLORED(FAIL, "User program exit with status %d\n"), status);
        }

        // TODO: release children

#ifdef USER_PROGRAM
        if (currentThread->space != NULL) {
#ifdef USE_BITMAP
            machine->freeMem(); // ONLY USE FOR TEST Lab4 Exercise4
#endif
            delete currentThread->space;
            currentThread->space = NULL;
        }
#endif
        // TODO: if it has parent, then set this to zombie and signal
        currentThread->Finish();
    }
}
```

#### Build the Scenario

I've add `StartTwoThread()` in `code/userprog/progtest.cc`. And user can invoke this by using `-X` flag that I've add the functionality in `code/threads/main.cc`.

This is the hardest part so far. Because it's hard to understand how the simulator (i.e. `Machine`) work when executing user program that `Machine::Run` will run in infinity loop.

Thus for now we need to make sure every user program exit properly.

On the top level. Just after use `-X` to execute user program.
The difference between original `StartProcess` is that this will create two threads using the same user program.

```c
//----------------------------------------------------------------------
// StartTwoThread
// 	Run a user program.  Open the executable, load it into
//	memory, create two copy of the thread and jump to it.
//  (ps. use -X filename, detail information is in thread/main.c)
//----------------------------------------------------------------------

void
StartTwoThread(char *filename)
{
    OpenFile *executable = fileSystem->Open(filename);

    if (executable == NULL) {
	    printf("Unable to open file %s\n", filename);
	    return;
    }

    Thread *thread1 = CreateSingleThread(executable, 1);
    Thread *thread2 = CreateSingleThread(executable, 2);

    delete executable;			// close file

    thread1->Fork(UserProgThread, (void*)1);
    thread2->Fork(UserProgThread, (void*)2);

    currentThread->Yield();
}
```

When creating the thread, I've made the thread priority greater than main thread.
And assign the address space that created from the executable to the thread's space.

```c
//----------------------------------------------------------------------
// CreateSingleThread
// 	Run a user program.  Open the executable, load it into
//	memory, create a copy of it and return the thread.
//----------------------------------------------------------------------

Thread*
CreateSingleThread(OpenFile *executable, int number)
{
    printf("Creating user program thread %d\n", number);

    char ThreadName[20];
    sprintf(ThreadName, "User program %d", number);
    Thread *thread = new Thread(strdup(ThreadName), -1);

    AddrSpace *space;
    space = new AddrSpace(executable);
    thread->space = space;

    return thread;
}
```

And for each user program thread, because the thread will become `currentThread`.
We need to initialize the machine register (use `AddrSpace::InitRegisters`) and load the page table (use `AddrSpace::RestoreState`). Then we're ready to go!

```c
//----------------------------------------------------------------------
// UserProgThread
// 	A basic user program thread.
//----------------------------------------------------------------------

void
UserProgThread(int number)
{
    printf("Running user program thread %d\n", number);
    currentThread->space->InitRegisters();		// set the initial register values
    currentThread->space->RestoreState();		// load page table register
    currentThread->space->PrintState(); // debug usage
    machine->Run();	// jump to the user progam
    ASSERT(FALSE);			// machine->Run never returns;
                // the address space exits
                // by doing the syscall "exit"
}
```

#### Modify Address Space for Context Switch

When context switch, the TLB will fail because of using different address space.

> But at this moment this will have no influence,
> because another thread will not interrupt or happen context switch when one thread is running.

```c
//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState()
{
#ifdef USE_TLB // Lab4: Clean up TLB
    DEBUG('T', "Clean up TLB due to Context Switch!\n");
    for (int i = 0; i < TLBSize; i++) {
        machine->tlb[i].valid = FALSE;
    }
#endif
}
```

#### Test Exercise 5

I've made a simple `code/test/exit.c` user program to test.

```c
/* halt.c
 *	Simple program to test multi-thread user program (Lab 4)
 */

#include "syscall.h"

int
main()
{
    int i;
    for (i = 0; i < 100; i++) {
        // do nothing
    }
    Exit(87);
}
```

And here is the result. (Debug message `S` for syscall, `T` for TLB, `t` for thread)

```txt
$ docker run nachos_userprog nachos/nachos-3.4/code/userprog/nachos -d STt -X nachos/nachos-3.4/code/test/exit
Creating user program thread 1
Creating user program thread 2
Forking thread "User program 1" with func = 0x80502b7, arg = 1
Putting thread User program 1 on ready list.
Forking thread "User program 2" with func = 0x80502b7, arg = 2
Putting thread User program 2 on ready list.
Yielding thread "main"
Putting thread main on ready list.
Switching from thread "main" to thread "User program 1"
Running user program thread 1
=== Address Space Information ===
numPages = 11
VPN     PPN     valid   RO      use     dirty
0       0       1       0       0       0
1       1       1       0       0       0
2       2       1       0       0       0
3       3       1       0       0       0
4       4       1       0       0       0
5       5       1       0       0       0
6       6       1       0       0       0
7       7       1       0       0       0
8       8       1       0       0       0
9       9       1       0       0       0
10      10      1       0       0       0
=================================
TLB Miss: 4, TLB Hit: 1322, Total Instruction: 1326, Total Translate: 1330, TLB Miss Rate: 0.30%
User program exit with status 87
Finishing thread "User program 1"
Sleeping thread "User program 1"
Switching from thread "User program 1" to thread "User program 2"
Running user program thread 2
=== Address Space Information ===
numPages = 11
VPN     PPN     valid   RO      use     dirty
0       11      1       0       0       0
1       12      1       0       0       0
2       13      1       0       0       0
3       14      1       0       0       0
4       15      1       0       0       0
5       16      1       0       0       0
6       17      1       0       0       0
7       18      1       0       0       0
8       19      1       0       0       0
9       20      1       0       0       0
10      21      1       0       0       0
=================================
TLB Miss: 4, TLB Hit: 2647, Total Instruction: 2651, Total Translate: 2655, TLB Miss Rate: 0.15%
User program exit with status 87
Finishing thread "User program 2"
Sleeping thread "User program 2"
Switching from thread "User program 2" to thread "main"
Now in thread "main"
Deleting thread "User program 2"
Finishing thread "main"
Sleeping thread "main"
No threads ready or runnable, and no pending interrupts.
Assuming the program completed.
Machine halting!

Ticks: total 2104, idle 0, system 60, user 2044
```

> Improvement:
>
> * User space thread should be able to switch to each other when executing (maybe running with `-rs` or `-rr`).
> * Seems that the memory didn't load as we expect. Because of the original memory is loaded sequentially. But actually we need to load it on what the bitmap allocate to us. (in `AddrSpace::AddrSpace`)
> * Understand how `Machine::Run` work (why run multiple time (for each thread))

### Exercise 6: Missing page interrupt handling

> Based on TLB mechanism exception handling and the replacement algorithm. Implement missing page interrupt handler and page replacement algorithm.
>
> (The TLB exception mechanism is loading the page in memory from memory to TLB. Thus, missing page handler is to load new page from disk to memory)

We need to generate page fault, that is the page in page table is invalid (unloaded) and we load it from disk to memory.

#### Virtual Memory

So I use a file to act as "virtual memory" that contain the file's code and data segment (if any).

And here is the modification on creating address space (`AddrSpace::AddrSpace`) (define `DEMAND_PAGING` to enable the code, see more explanaiton [here](#III.-Lazy-loading-(i.e.-Demand-Paging)))

```c
AddrSpace::AddrSpace(OpenFile *executable)
{
    ...

    // Create a virtual memory with the size that the executable file need.
    DEBUG('a', "Demand paging: creating virtual memory!\n");
    bool success_create_vm = fileSystem->Create("VirtualMemory", size);
    ASSERT_MSG(success_create_vm, "fail to create virtual memory");

    // Initialize page table
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
        ...

        pageTable[i].valid = FALSE;

        ...
    }

    bzero(machine->mainMemory, MemorySize);

    DEBUG('a', "Demand paging: copy executable to virtual memory!\n");

    OpenFile *vm = fileSystem->Open("VirtualMemory");

    char *virtualMemory_temp;
    virtualMemory_temp = new char[size];
    for (i = 0; i < size; i++)
        virtualMemory_temp[i] = 0;

    if (noffH.code.size > 0) {
        DEBUG('a', "\tCopying code segment, at 0x%x, size %d\n",
              noffH.code.virtualAddr, noffH.code.size);
        executable->ReadAt(&(virtualMemory_temp[noffH.code.virtualAddr]),
                           noffH.code.size, noffH.code.inFileAddr);
        vm->WriteAt(&(virtualMemory_temp[noffH.code.virtualAddr]),
                    noffH.code.size, noffH.code.virtualAddr*PageSize);
    }
    if (noffH.initData.size > 0) {
        DEBUG('a', "\tCopying data segment, at 0x%x, size %d\n",
              noffH.initData.virtualAddr, noffH.initData.size);
        executable->ReadAt(&(virtualMemory_temp[noffH.initData.virtualAddr]),
                           noffH.initData.size, noffH.initData.inFileAddr);
        vm->WriteAt(&(virtualMemory_temp[noffH.initData.virtualAddr]),
                    noffH.initData.size, noffH.initData.virtualAddr*PageSize);
    }
    delete vm; // Close the file
}
```

Here is the explination of the changes

1. `pageTable[i].valid = FALSE;` means we don't load any physical segment from disk to page. (you can see the first page will page fault on [later test](#Test-with-user-program)). But it's okay to load the fist few pages at first to reduce the page fault rate.
2. I've created a file call "VirtualMemory". This will be created at where the `nachos` executable at (the root directory, in this case `code/userprog/`).
   * Copy the code and data segment from "executable" to "vm"

> Note that, when the position offset of "executable" is `noffH.code.inFileAddr`, where we need to write in virtual memory is `noffH.code.virtualAddr*PageSize`.
> Because in real file there is a header. But in virtual memory, there are only code and data segments!
>
> (If miss this will mess the instruction decode, and probabily will cause infinity loop!)

#### Missing Page Handling

In the TLB exercise, I've left a space for Missing Page page fault in `code/userprog/exception.cc`.

When we failed to find a valid page, it will cause missing page page fault.

```c
void
TLBMissHandler(int virtAddr)
{
    unsigned int vpn;
    vpn = (unsigned) virtAddr / PageSize;

    // Find the Page
    TranslationEntry page = machine->pageTable[vpn];
    if (!page.valid) { // Lab4: Demand paging
        DEBUG('m', COLORED(WARNING, "\t=> Page miss\n"));
        page = PageFaultHandler(vpn);
    }

    ...
```

And here is how the `PageFaultHandler` do:

1. Find an empty space in memory with `machine->allocateFrame()` (this is implemented by [Exercise 4](#Exercise-4:-Global-data-structure-for-memory-management))
2. Load the page frame from disk to memory
    * If memory out of space then find a page to replace using `NaivePageReplacement`. This will be explain in the [Exercise 7](#Naive-Page-Replacement)

```c
TranslationEntry
PageFaultHandler(int vpn)
{
    // Get a Memory space (page frame) to allocate
    int pageFrame = machine->allocateFrame(); // ppn
    if (pageFrame == -1) { // Need page replacement
        pageFrame = NaivePageReplacement(vpn);
    }
    machine->pageTable[vpn].physicalPage = pageFrame;

    // Load the Page from virtual memory
    DEBUG('a', "Demand paging: loading page from virtual memory!\n");
    OpenFile *vm = fileSystem->Open("VirtualMemory"); // This file is created in userprog/addrspace.cc
    ASSERT_MSG(vm != NULL, "fail to open virtual memory");
    vm->ReadAt(&(machine->mainMemory[pageFrame*PageSize]), PageSize, vpn*PageSize);
    delete vm; // close the file

    // Set the page attributes
    machine->pageTable[vpn].valid = TRUE;
    machine->pageTable[vpn].use = FALSE;
    machine->pageTable[vpn].dirty = FALSE;
    machine->pageTable[vpn].readOnly = FALSE;

    currentThread->space->PrintState(); // debug with -d M to show bitmap
}
```

## III. Lazy-loading (i.e. Demand Paging)

Notice that in `code/userprog/Makefile` it enable micros `DEFINES = -DFILESYS_NEEDED -DFILESYS_STUB` on default.
That we need to use "disk" to build the scenario to make the "page fault"

I've also made the `-DDEMAND_PAGING` to enable demand paging when we need it.

> Use `-d s` to enable single step debug
>
> `docker run -it nachos_userprog nachos/nachos-3.4/code/userprog/nachos -s -d amM -x nachos/nachos-3.4/code/test/halt`

### Exercise 7: Loading page on demand

> Nachos allocate memory must be completed once the user program is loaded into memory. Thus the user program size is strictly restrict to be lower than 4KB.
> Implement a lazy-loading algorithm that load the page from disk to memory if and only if the missing page exception occur.

#### Naive Page Replacement

> This is the continus part of the [last Experiment](#Missing-Page-Handling)

1. Find an non-dirty page frame to replace.
2. If not found, then replace a dirty page and write back to disk.
3. Return the page frame number when founded or after replacement.

```c
int
NaivePageReplacement(int vpn)
{
    int pageFrame = -1;
    for (int temp_vpn = 0; temp_vpn < machine->pageTableSize, temp_vpn != vpn; temp_vpn++) {
        if (machine->pageTable[temp_vpn].valid) {
            if (!machine->pageTable[temp_vpn].dirty) {
                pageFrame = machine->pageTable[temp_vpn].physicalPage;
                break;
            }
        }
    }
    if (pageFrame == -1) { // No non-dirty entry
        for (int replaced_vpn = 0; replaced_vpn < machine->pageTableSize, replaced_vpn != vpn; replaced_vpn++) {
            if (machine->pageTable[replaced_vpn].valid) {
                machine->pageTable[replaced_vpn].valid = FALSE;
                pageFrame = machine->pageTable[replaced_vpn].physicalPage;

                // Store the page back to disk
                OpenFile *vm = fileSystem->Open("VirtualMemory");
                ASSERT_MSG(vm != NULL, "fail to open virtual memory");
                vm->WriteAt(&(machine->mainMemory[pageFrame*PageSize]), PageSize, replaced_vpn*PageSize);
                delete vm; // close file
                break;
            }
        }
    }
    return pageFrame;
}
```

#### Test with user program

Halt user program

```txt
$ docker run -it nachos_userprog nachos/nachos-3.4/code/userprog/nachos -d TM -x nachos/nachos-3.4/code/test/halt
Allocate physical page frame: 0
=== Address Space Information ===
numPages = 10
VPN     PPN     valid   RO      use     dirty
0       0       1       0       0       0
1       0       0       0       0       0
2       0       0       0       0       0
3       0       0       0       0       0
4       0       0       0       0       0
5       0       0       0       0       0
6       0       0       0       0       0
7       0       0       0       0       0
8       0       0       0       0       0
9       0       0       0       0       0
Current Bitmap: 00000001
=================================
Allocate physical page frame: 1
=== Address Space Information ===
numPages = 10
VPN     PPN     valid   RO      use     dirty
0       0       1       0       0       0
1       1       1       0       0       0
2       0       0       0       0       0
3       0       0       0       0       0
4       0       0       0       0       0
5       0       0       0       0       0
6       0       0       0       0       0
7       0       0       0       0       0
8       0       0       0       0       0
9       0       0       0       0       0
Current Bitmap: 00000003
=================================
Allocate physical page frame: 2
=== Address Space Information ===
numPages = 10
VPN     PPN     valid   RO      use     dirty
0       0       1       0       0       0
1       1       1       0       0       0
2       0       0       0       0       0
3       0       0       0       0       0
4       0       0       0       0       0
5       0       0       0       0       0
6       0       0       0       0       0
7       0       0       0       0       0
8       0       0       0       0       0
9       2       1       0       0       0
Current Bitmap: 00000007
=================================
TLB Miss: 6, TLB Hit: 11, Total Instruction: 17, Total Translate: 23, TLB Miss Rate: 35.29%
Machine halting!

Ticks: total 28, idle 0, system 10, user 18
```

Exit user program

```txt
$ docker run -it nachos_userprog nachos/nachos-3.4/code/userprog/nachos -d TM -x nachos/nachos-3.4/code/test/exit
Allocate physical page frame: 0
=== Address Space Information ===
numPages = 11
VPN     PPN     valid   RO      use     dirty
0       0       1       0       0       0
1       0       0       0       0       0
2       0       0       0       0       0
3       0       0       0       0       0
4       0       0       0       0       0
5       0       0       0       0       0
6       0       0       0       0       0
7       0       0       0       0       0
8       0       0       0       0       0
9       0       0       0       0       0
10      0       0       0       0       0
Current Bitmap: 00000001
=================================
Allocate physical page frame: 1
=== Address Space Information ===
numPages = 11
VPN     PPN     valid   RO      use     dirty
0       0       1       0       0       0
1       1       1       0       0       0
2       0       0       0       0       0
3       0       0       0       0       0
4       0       0       0       0       0
5       0       0       0       0       0
6       0       0       0       0       0
7       0       0       0       0       0
8       0       0       0       0       0
9       0       0       0       0       0
10      0       0       0       0       0
Current Bitmap: 00000003
=================================
Allocate physical page frame: 2
=== Address Space Information ===
numPages = 11
VPN     PPN     valid   RO      use     dirty
0       0       1       0       0       0
1       1       1       0       0       0
2       0       0       0       0       0
3       0       0       0       0       0
4       0       0       0       0       0
5       0       0       0       0       0
6       0       0       0       0       0
7       0       0       0       0       0
8       0       0       0       0       0
9       0       0       0       0       0
10      2       1       0       0       0
Current Bitmap: 00000007
=================================
Allocate physical page frame: 3
=== Address Space Information ===
numPages = 11
VPN     PPN     valid   RO      use     dirty
0       0       1       0       0       0
1       1       1       0       0       0
2       3       1       0       0       0
3       0       0       0       0       0
4       0       0       0       0       0
5       0       0       0       0       0
6       0       0       0       0       0
7       0       0       0       0       0
8       0       0       0       0       0
9       0       0       0       0       0
10      2       1       0       0       0
Current Bitmap: 0000000F
=================================
TLB Miss: 8, TLB Hit: 1319, Total Instruction: 1327, Total Translate: 1335, TLB Miss Rate: 0.60%
Free physical page frame: 0
Free physical page frame: 1
Free physical page frame: 3
Free physical page frame: 2
Bitmap after freed: 00000000
No threads ready or runnable, and no pending interrupts.
Assuming the program completed.
Machine halting!

Ticks: total 1038, idle 0, system 10, user 1028
```

