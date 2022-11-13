// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void
SwapHeader(NoffHeader *noffH) {
    noffH->noffMagic = WordToHost(noffH->noffMagic);
    noffH->code.size = WordToHost(noffH->code.size);
    noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
    noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
    noffH->initData.size = WordToHost(noffH->initData.size);
    noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
    noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
    noffH->uninitData.size = WordToHost(noffH->uninitData.size);
    noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
    noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

BitMap *AddrSpace::freeMap = new BitMap(NumPhysPages);
BitMap *AddrSpace::swapMap = new BitMap(NumPhysPages);
OpenFile *AddrSpace::swapFile = fileSystem->Open("SWAP");

AddrSpace::AddrSpace(OpenFile *executable) {
    for (int i = 0; i < 128; i++) {
        if (spaceIDs[i] == 0) {
            spaceID = i;
            spaceIDs[i] = 1;
            break;
        }
    }


    NoffHeader noffH;
    unsigned int i, size;

    this->executable = executable;

    executable->ReadAt((char *) &noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) &&
        (WordToHost(noffH.noffMagic) == NOFFMAGIC))
        SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

    numPages = divRoundUp(noffH.code.size, PageSize) + divRoundUp(noffH.initData.size, PageSize) +
               divRoundUp(noffH.uninitData.size, PageSize) + StackPages;
    size = numPages * PageSize;

    printf("the NumPhysPages is %d\n", NumPhysPages);
    printf("the numPages is %d\n", numPages);

    DEBUG('a', "Initializing address space, num pages %d, size %d\n",
          numPages, size);

    bzero(machine->mainMemory, size);
    InitPageTable();
    InitInFileAddr();
    print();
    printf("end\n");
}

AddrSpace::~AddrSpace() {
    for (int i = 0; i < numPages; i++)
        freeMap->Clear(pageTable[i].physicalPage);
    delete[] pageTable;
}

void
AddrSpace::InitRegisters() {
    int i;
    for (i = 0; i < NumTotalRegs; i++)
        machine->WriteRegister(i, 0);
    machine->WriteRegister(PCReg, 0);
    machine->WriteRegister(NextPCReg, 4);
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

void AddrSpace::SaveState() {
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}

void AddrSpace::RestoreState() {
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}

void AddrSpace::print() {

    printf("spaceID: %d\n", spaceID);
    printf("page table dump %d pages in total\n", numPages);
    printf("====================================\n");
    printf("\tVirtPage,\tPhysPage \tuse \tvalid \tdirty \n");
    for (int i = 0; i < numPages; i++) {
        printf("\t%d,\t\t%d \t\t%d \t%d \t%d\n", pageTable[i].virtualPage, pageTable[i].physicalPage, pageTable[i].use,
               pageTable[i].valid, pageTable[i].dirty);
    }
    printf("====================================\n");
}

void AddrSpace::InitPageTable() {
    p_vm = 0;
    pageTable = new TranslationEntry[numPages];
    for (int i = 0; i < numPages; i++) {
        pageTable[i].virtualPage = i;
        pageTable[i].use = false;
        pageTable[i].dirty = false;
        pageTable[i].readOnly = false;
        pageTable[i].inFileAddr = -1;
        if (i >= numPages - StackPages)
            pageTable[i].type = vmuserStack;
        if (i < MemPages) {
            virtualMem[p_vm] = pageTable[i].virtualPage;
            p_vm = (p_vm + 1) % MemPages;
            pageTable[i].physicalPage = freeMap->Find();
            pageTable[i].valid = true;
        } else {
            pageTable[i].physicalPage = -1;
            pageTable[i].valid = false;
        }
    }
}

void AddrSpace::InitInFileAddr() {
    NoffHeader noffH;
    executable->ReadAt((char *) &noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) &&
        (WordToHost(noffH.noffMagic) == NOFFMAGIC))
        SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);
    if (noffH.code.size > 0) {
        unsigned int numP = divRoundUp(noffH.code.size, PageSize);
        for (int i = 0; i < numP; i++) {
            pageTable[i].inFileAddr = noffH.code.inFileAddr + i * PageSize;
            pageTable[i].type = vmcode;
            if (pageTable[i].valid) {
                executable->ReadAt(&(machine->mainMemory[pageTable[i].physicalPage * PageSize]), PageSize,
                                   pageTable[i].inFileAddr);
            }
        }
    }
    if (noffH.initData.size > 0) {
        unsigned int numP, firstP;
        numP = divRoundUp(noffH.initData.size, PageSize);
        firstP = divRoundUp(noffH.initData.virtualAddr, PageSize);
        for (int i = firstP; i < numP + firstP; i++) {
            pageTable[i].inFileAddr = noffH.initData.inFileAddr + (i - firstP) * PageSize;
            pageTable[i].type = vminitData;
            if (pageTable[i].valid) {
                executable->ReadAt(&(machine->mainMemory[pageTable[i].physicalPage * PageSize]), PageSize,
                                   pageTable[i].inFileAddr);
            }
        }
    }
    if (noffH.uninitData.size > 0) {
        unsigned int numP, firstP;
        numP = divRoundUp(noffH.uninitData.size, PageSize);
        firstP = divRoundUp(noffH.uninitData.virtualAddr, PageSize);
        for (int i = firstP; i < numP + firstP; i++) {
            pageTable[i].type = vmuninitData;

        }
    }
}

void AddrSpace::Translate(int addr, unsigned int *vpn, unsigned int *offset) {
    printf("enter translate\n");
    NoffHeader noffH;
    executable->ReadAt((char *) &noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) &&
        (WordToHost(noffH.noffMagic) == NOFFMAGIC))
        SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);
    int page = -1;
    int off = 0;
    if (addr >= numPages * PageSize - UserStackSize) {
        int userPages = numPages - StackPages;
        page = userPages + (addr - userPages * PageSize) / PageSize;
        off = (addr - userPages * PageSize) % PageSize;
    } else if (noffH.uninitData.size > 0 && addr >= noffH.uninitData.virtualAddr) {
        page = divRoundUp(noffH.code.size, PageSize) + divRoundUp(noffH.initData.size, PageSize) +
               (addr - noffH.uninitData.virtualAddr) % PageSize;
        off = (addr - noffH.uninitData.virtualAddr) % PageSize;
    } else if (noffH.initData.size > 0 && addr >= noffH.initData.virtualAddr) {
        page = divRoundUp(noffH.code.size, PageSize) + (addr - noffH.initData.virtualAddr) / PageSize;
        off = (addr - noffH.initData.virtualAddr) % PageSize;
    } else {
        page = addr / PageSize;
        off = addr % PageSize;
    }
    *vpn = page;
    *offset = off;
}

void AddrSpace::FIFO(int badVAddr) {
    printf("enter fifo\n");
    unsigned int oldPage = virtualMem[p_vm];
    unsigned int newPage;
    unsigned int tmp;
    Translate(badVAddr, &newPage, &tmp);
    ASSERT(newPage < numPages);

    virtualMem[p_vm] = newPage;
    p_vm = (p_vm + 1) % MemPages;
    printf("swap vm page %d:%d=>%d\n", pageTable[oldPage].physicalPage, pageTable[oldPage].virtualPage,
           pageTable[newPage].virtualPage);
    Swap(oldPage, newPage);
}

void AddrSpace::Swap(int oldPage, int newPage) {
    printf("enter swap\n");
    WriteBack(oldPage);
    pageTable[newPage].physicalPage = pageTable[oldPage].physicalPage;
    pageTable[oldPage].physicalPage = -1;
    pageTable[oldPage].valid = FALSE;
    pageTable[newPage].valid = true;
    pageTable[newPage].use = true;
    pageTable[newPage].dirty = false;
    ReadIn(newPage);
    print();
}

void AddrSpace::WriteBack(int oldPage) {
    printf("enter writeBack\n");
    if (pageTable[oldPage].dirty) {
        switch (pageTable[oldPage].type) {
            case vmcode:
            case vminitData:
                ASSERT(pageTable[oldPage].type != vmcode);
                ASSERT(pageTable[oldPage].type != vminitData);
                executable->WriteAt(&(machine->mainMemory[pageTable[oldPage].physicalPage * PageSize]), PageSize,
                                    pageTable[oldPage].inFileAddr);
                break;
            case vmuninitData:
            case vmuserStack:
                pageTable[oldPage].inFileAddr =
                        (swapMap->FindIn(spaceID * NumPhysPages, (spaceID + 1) * NumPhysPages)) * PageSize;
                swapFile->WriteAt(&(machine->mainMemory[pageTable[oldPage].physicalPage * PageSize]), PageSize,
                                  pageTable[oldPage].inFileAddr);
                break;
        }
        pageTable[oldPage].dirty = false;
    }
}

void AddrSpace::ReadIn(int newPage) {
    printf("enter readin\n");
    switch (pageTable[newPage].type) {
        case vmcode:
        case vminitData:
            printf("copy from source file pageTable[newPage].inFileAddr:%d===>mainMemory[%d]\n",
                   pageTable[newPage].inFileAddr, pageTable[newPage].physicalPage * PageSize);
            executable->ReadAt(&(machine->mainMemory[pageTable[newPage].physicalPage * PageSize]), PageSize,
                               pageTable[newPage].inFileAddr);
            break;
        case vmuninitData:
            if (pageTable[newPage].inFileAddr >= 0) {
                printf("copy from swap file pageTable[newPage].inFileAddr:%d===>mainMemory[%d]\n",
                       pageTable[newPage].inFileAddr, pageTable[newPage].physicalPage * PageSize);
                swapFile->ReadAt(&(machine->mainMemory[pageTable[newPage].physicalPage * PageSize]), PageSize,
                                 pageTable[newPage].inFileAddr);
                swapMap->Clear(pageTable[newPage].inFileAddr / PageSize);
                pageTable[newPage].inFileAddr = -1;
            } else
                bzero(machine->mainMemory + pageTable[newPage].physicalPage * PageSize, PageSize);
            break;

    }
}