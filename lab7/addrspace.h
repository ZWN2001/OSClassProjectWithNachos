#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "bitmap.h"
#define UserStackSize		1024 	// increase this as necessary!
#define StackPages UserStackSize/PageSize
#define MemPages 5

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
    void FIFO(int newPage);//FIFO swap
    void Translate(int addr,unsigned int *vpn,unsigned int *offset);
    void Swap(int oldPage,int newPage);

    void WriteBack(int oldPage);
    void ReadIn(int newPage);
    void PrintVM();
private:
    TranslationEntry *pageTable;
    unsigned int numPages;
    static BitMap *freeMap;
    int spaceID;
    int virtualMem[NumPhysPages];
    int p_vm;
};

#endif // ADDRSPACE_H
