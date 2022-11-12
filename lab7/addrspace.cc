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
#include "bitmap.h"

static BitMap *pageMap = new BitMap(NumPhysPages);
static BitMap *pidMap = new BitMap(NumPhysPages);


//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
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
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(char *filename)
{
    NoffHeader noffH;
    unsigned int i, size, j;
    OpenFile *executable = fileSystem->Open(filename);

    if (executable == NULL) {
        printf("Unable to open file %s\n", filename);
        return;
    }
    this->filename=filename;
    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size
           + UserStackSize;
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

    ASSERT(numPages <= NumPhysPages);

    printf("Initializing address space, num pages %d, size %d\n",
					numPages, size);

	if(numPages > pageMap->NumClear()){
		printf("Not enough pages in pageMap.");
		return;
	}
	ASSERT(numPages <= (unsigned int) pageMap->NumClear());

    {
        spaceId = pidMap->Find()+100;
        pageTable = new TranslationEntry[numPages];
        pagePointer = 0;
        for (i = 0; i < MaxNumPhysPages; i++) {
            pageTable[i].virtualPage = i;
            pageTable[i].physicalPage = pageMap->Find();
            pageTable[i].valid = TRUE;
            pageTable[i].use = FALSE;
            pageTable[i].dirty = FALSE;
            pageTable[i].readOnly = FALSE;
            virtualMem[pagePointer] = pageTable[i].virtualPage;
            pagePointer = (pagePointer + 1) % MaxNumPhysPages; //向前移一位
        }
        for(i;i<numPages;i++){
            pageTable[i].virtualPage = i;
            pageTable[i].physicalPage = -1;
            pageTable[i].valid = false;
        }
    }



//	for (i = 0; i < numPages; i++){
//		for (j = 0; j < PageSize; j++)
//			machine->mainMemory[ pageTable[i].physicalPage * PageSize + j ] = 0;
//	}

	RestoreState();
//    if (noffH.code.size > 0) {
//        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",
//			noffH.code.virtualAddr, noffH.code.size);
//		int phyAddr;
//		machine->Translate(noffH.code.virtualAddr, &phyAddr, 4, TRUE);
//        executable->ReadAt(&(machine->mainMemory[phyAddr]),	//may out of bound
//			noffH.code.size, noffH.code.inFileAddr);
//    }
//    if (noffH.initData.size > 0) {
//        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n",
//			noffH.initData.virtualAddr, noffH.initData.size);
//		int phyAddr;
//		machine->Translate(noffH.initData.virtualAddr, &phyAddr, 4, TRUE);
//        executable->ReadAt(&(machine->mainMemory[phyAddr]),	//may out of bound
//			noffH.initData.size, noffH.initData.inFileAddr);
//    }
    if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",
              noffH.code.virtualAddr, noffH.code.size);

        int code_page = noffH.code.virtualAddr/PageSize;
        int code_phy_addr = pageTable[code_page].physicalPage * PageSize;
        executable->ReadAt(&(machine->mainMemory[code_phy_addr]),
                           noffH.code.size, noffH.code.inFileAddr);
    }

    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n",
              noffH.initData.virtualAddr, noffH.initData.size);
        int data_page = noffH.initData.virtualAddr/PageSize;
        int data_offset = noffH.initData.virtualAddr % PageSize;
        int data_phy_addr = pageTable[data_page].physicalPage * PageSize + data_offset;
        executable->ReadAt(&(machine->mainMemory[data_phy_addr]),
                           noffH.initData.size, noffH.initData.inFileAddr);
    }
    delete executable;	// 关闭文件
}


//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space and reset the bit map
//
// Implemented by Bluefissure.
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
	unsigned int i;
    pidMap->Clear(spaceId-100);
	for (i = 0; i < numPages; i++){	//reset the bitmap
        if(pageTable[i].valid){
            pageMap->Clear(pageTable[i].physicalPage);
        }
	}
	delete [] pageTable;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;
    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);
    machine->WriteRegister(PCReg, 0);
    machine->WriteRegister(NextPCReg, 4);
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//	numPagesReg -- number of pages to store registers
// Implemented by Bluefissure.
//----------------------------------------------------------------------

void AddrSpace::SaveState() //Save register to memory and save memory
{
    pageTable = machine->pageTable;
    numPages = machine->pageTableSize;
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//	Restore the registers stored in memery 
//		and tell machine to find current pagetable
// Implemented by Bluefissure.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}

//----------------------------------------------------------------------
// AddrSpace::Print
//  Page tables dumping 
// Implemented by Bluefissure
//----------------------------------------------------------------------

void AddrSpace::Print(){
    printf("spaceID: %d\n",spaceId);
    printf("page table dump: %d pages in total\n", numPages);
    printf("============================================\n");
    printf(" VirtPage, PhysPage, valid, dirty, use \n");
    for (int i=0; i < numPages; i++) {
        printf("\t%d,\t%d,\t%d,\t%d,\t%d\n", pageTable[i].virtualPage,pageTable[i].physicalPage,pageTable[i].valid,pageTable[i].dirty,pageTable[i].use);
    }
    printf("============================================\n\n");
}

//先通过dirty为判断是否已经改动过，如果true，写回磁盘
void AddrSpace::writeBack(int oldPage)
{
    if(pageTable[oldPage].dirty){
        OpenFile *executable = fileSystem->Open(filename);
        if (executable == NULL) {
            printf("Unable to open file %s\n", filename);
            return;
        }
        executable->WriteAt(&(machine->mainMemory[pageTable[oldPage].physicalPage]),PageSize,oldPage*PageSize);
        delete executable;
    }
}

void AddrSpace::FIFO(int badVAddr)
{
    int newPage=badVAddr/PageSize;
    if(newPage > numPages){
        printf("newPage:%d",newPage);
        return;
    }
    printf("需换入newPage: %d\n",newPage);
    int oldPage = virtualMem[pagePointer];
    printf("换出页oldPage: %d\n",oldPage);

    virtualMem[pagePointer] = newPage;
    pagePointer = (pagePointer + 1) % MaxNumPhysPages;
    //写回
    writeBack(oldPage);
    pageTable[oldPage].valid = false;
    pageTable[newPage].physicalPage = pageTable[oldPage].physicalPage;
    pageTable[oldPage].physicalPage = -1;
    pageTable[newPage].valid = true;
    pageTable[newPage].dirty = false;
    pageTable[newPage].readOnly = false;

    OpenFile *executable = fileSystem->Open(filename);

    if (executable == NULL) {
        printf("Unable to open file %s\n", filename);
        return;
    }
    executable->ReadAt(&(machine->mainMemory[pageTable[newPage].physicalPage]),PageSize, newPage*PageSize);
    delete executable;

    Print();
}

