// addrspace.h 
//	Data structures to keep track of executing user programs 
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
///目前，我们不保留任何有关地址空间的信息。
///用户级 CPU 状态保存在执行用户程序的线程中(参见 thread.h)。

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"

#define UserStackSize		1024 	// increase this as necessary!

class AddrSpace {
  public:
    ///创建并初始化地址空间，并存出现在打开的文件中
    AddrSpace(OpenFile *executable);	// Create an address space,
					// initializing it with the program
					// stored in the file "executable"
    ~AddrSpace();			// De-allocate an address space

    ///在进入用户代码前初始化用户级CPU 寄存器
    void InitRegisters();		// Initialize user-level CPU registers,
					// before jumping to user code

    ///存或者重存特定地址空间
    ///在上下文切换时保存机器状态
    void SaveState();			// Save/restore address space-specific
    ///上下文切换后恢复状态
    void RestoreState();		// info on a context switch 

  private:
    ///页表，线性数组
    TranslationEntry *pageTable;	// Assume linear page table translation
					// for now!
    ///虚拟地址空间的页数
    unsigned int numPages;		// Number of pages in the virtual address space

};

#endif // ADDRSPACE_H
