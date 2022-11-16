#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"

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

BitMap *AddrSpace::freeMap = new BitMap(NumPhysPages);
//BitMap *AddrSpace::swapMap = new BitMap(NumPhysPages);
//OpenFile *AddrSpace::swapFile = fileSystem->Open("SWAP");
//OpenFile *AddrSpace::swapFile;

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
    executable->ReadAt((char *) &noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) &&
        (WordToHost(noffH.noffMagic) == NOFFMAGIC))
        SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStackSize;
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;
    fileSystem->Remove("VirtualMemory");
    fileSystem->Create("VirtualMemory", size);

    InitPageTable();

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
    delete vm;

    print();
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
        if (i < MemPages) {
            virtualMem[p_vm] = -1;
            p_vm = (p_vm + 1) % MemPages;
        }
        pageTable[i].physicalPage = -1;
        pageTable[i].valid = false;
    }
}

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

void AddrSpace::PrintVM(){
    for(int i = 0;i <MemPages;i++){
        printf("%d, ",virtualMem[i]);
    }
    printf("\n");
}

void AddrSpace::Swap(int oldPage, int newPage) {
    printf("enter swap\n");
    PrintVM();
    if(oldPage == -1){
        if(p_vm == 0){
            pageTable[newPage].physicalPage = 4;
        } else{
            pageTable[newPage].physicalPage = p_vm - 1;
        }
    } else{
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
        vm->WriteAt(&(machine->mainMemory[pageTable[oldPage].physicalPage * PageSize]), PageSize,
                    pageTable[oldPage].virtualPage * PageSize);
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
