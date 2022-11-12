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

#define UserStackSize	1024
#define MaxNumPhysPages  5

class AddrSpace {
  public:
    AddrSpace(char *filename);
    ~AddrSpace();

    void InitRegisters();

    void SaveState();
    void RestoreState();

	void Print();
    unsigned int getSpaceId() { return spaceId; }

    void FIFO(int badVAdrr);
    void writeBack(int oldPage);


  private:
    TranslationEntry *pageTable;
    unsigned int numPages, spaceId;

    unsigned int StackPages;
    char *filename;
    //for FIFO
    int virtualMem[NumPhysPages]; //FIFO页顺序存储
    int pagePointer; //FIFO换出页指针

};

#endif
