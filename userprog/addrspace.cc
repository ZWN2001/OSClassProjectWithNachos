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
///管理地址空间的例程(执行用户程序)。
///要运行用户程序，必须:
///1. 使用 -N-T 0 选项链接
///2. 运行 Coff2noff 将目标文件转换为 Nachos 格式
///(Nachos 对象代码格式实际上只是 UNIX 可执行对象代码格式的简单版本)
///3. 将 NOFF 文件加载到 Nachos 文件系统中
///(如果还没有实现文件系统，则不需要执行最后一步)

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
///对对象文件头中的字节执行从 little endian 到 big endian 转换，
/// 以防文件是在 little endian 机器上生成的，而在 big endian 机器上运行。
//----------------------------------------------------------------------

static void SwapHeader (NoffHeader *noffH){
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
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

    ASSERT(numPages <= NumPhysPages);		// check we're not trying
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
	pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
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

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
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

///此函数设置用户级寄存器的初始值。(machine/machine.*共定义了40个寄存器)
/// 我们把直接写入寄存器，这样我们就可以立即跳转到用户代码。
/// 具体来说，程序计数器寄存器 PCReg 被初始化为0。
/// 请注意，当线程被切换出来时，这些将被保存/恢复到 currentThread-> userRegister 中。
void AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}
