// addrspace.h 
//	Data structures to keep track of executing user programs 
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"

#define UserStackSize		1024 	// increase this as necessary!

class AddrSpace {
  public:
    AddrSpace(OpenFile *executable);	// Create an address space,
					// initializing it with the program
					// stored in the file "executable"
    ~AddrSpace();			// De-allocate an address space

    void InitRegisters();		// Initialize user-level CPU registers,
					// before jumping to user code

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch 

	void Print();	//page table dumping
    unsigned int getSpaceId() { return spaceId; }

//	void setPreAddrSpace(AddrSpace *preAddrSpace);		// Set pre addrspace
//	void setNextAddrSpace(AddrSpace *nextAddrSpace);	// Set next addrspace
//	AddrSpace* PreAddrSpace(){	// Get pre addrspace
//		return preAddrSpace;
//	};
//	AddrSpace* NextAddrSpace(){	// Get next addrspace
//		return nextAddrSpace;
//	}

  private:
    TranslationEntry *pageTable;	// Assume linear page table translation
//	TranslationEntry *regPageTable;	// Pagetable to store the registers
//	AddrSpace *preAddrSpace,*nextAddrSpace;	// Linked list
    unsigned int numPages, spaceId;		// Number of pages in the virtual
					// address space
};

#endif // ADDRSPACE_H