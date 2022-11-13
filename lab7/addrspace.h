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
#include "bitmap.h"
#define UserStackSize		1024 	// increase this as necessary!
#define StackPages UserStackSize/PageSize
#define MemPages 10

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
    int getSpaceID(){return spaceID;}
    void print();


    void InitPageTable();//init pageTable
    void InitInFileAddr();//init pageTable entry inFileAddr
    void FIFO(int newPage);//FIFO swap
    void Translate(int addr,unsigned int *vpn,unsigned int *offset);
    //addr vpn to virtualPageNumber,offset
    void Swap(int oldPage,int newPage);
    //use WriteBack and ReadIn be oldPage to newPage
    void WriteBack(int oldPage);
    void ReadIn(int newPage);
  private:
    TranslationEntry *pageTable;	// Assume linear page table translation
					// for now!
    unsigned int numPages;		// Number of pages in the virtual 
					// address space

    static BitMap *freeMap;
    int spaceID;

    static BitMap *swapMap;
    static OpenFile *swapFile;

    OpenFile *executable;
    int virtualMem[NumPhysPages];
    int p_vm;


};

#endif // ADDRSPACE_H
