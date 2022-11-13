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

#define UserStackSize 1024 // increase this as necessary!

// 建一个空闲空间的指针
static BitMap freeMap(128);

class AddrSpace
{
public:
  AddrSpace(OpenFile *executable);

  void InitRegisters();

  void SaveState();
  void RestoreState();

  void Print();
  unsigned int getSpaceId() { return spaceId; }

private:
  TranslationEntry *pageTable;
  unsigned int numPages;
  int spaceId;
};

#endif // ADDRSPACE_H
