// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------
void StartProcess(int spaceId){
    currentThread->space->InitRegisters();
    currentThread->space->RestoreState();
    machine->Run();
    ASSERT(FALSE);
}

void ExceptionHandler(ExceptionType which) {
    int type = machine->ReadRegister(2), i;
    if(which == PageFaultException){
        int badVAddr = machine->ReadRegister(BadVAddrReg);
        interrupt->PageFault(badVAddr);
        {
            machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
            machine->WriteRegister(PCReg, machine->ReadRegister(PCReg) + 4);
            machine->WriteRegister(NextPCReg, machine->ReadRegister(PCReg)+4);
        }
    } else if ((which == SyscallException) && (type == SC_Halt)) {
        DEBUG('a', "Shutdown, initiated by user program.\n");
        interrupt->Halt();
    } else if ((which == SyscallException) && (type == SC_Exec)) {
        int FilePathMaxLen = 100;
        char filepath[FilePathMaxLen];
        int fileaddr = machine->ReadRegister(4);
        for (i = 0; filepath[i] != '\0'; i++) {
            if (!machine->ReadMem(fileaddr + 4 * i, 4, (int *) (filepath + 4 * i)))
                break;
        }
//        OpenFile *executable = fileSystem->Open(filepath);
//        if(executable == NULL) {
//            printf("Unable to open file %s\n",filepath);
//            return;
//        }
        // 建立新地址空间
        AddrSpace *space = new AddrSpace(filepath);
//        delete executable;	// 关闭文件
        // 建立新核心线程
        Thread *thread = new Thread(filepath);
        // 将用户进程映射到核心线程上
        thread->space = space;
        space->Print();
        thread->Fork(StartProcess,(int)space->getSpaceId());
        machine->WriteRegister(2,space->getSpaceId());
        {
            machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
            machine->WriteRegister(PCReg, machine->ReadRegister(PCReg) + 4);
            machine->WriteRegister(NextPCReg, machine->ReadRegister(PCReg)+4);
        }
    } else if ((which == SyscallException) && (type == SC_Exit)) {
        int ExitCode = machine->ReadRegister(4);
        Exit(ExitCode);
    } else if ((which == SyscallException) && (type == SC_PrintInt)) {
        int val = machine->ReadRegister(4); //将MIPS machine存的参数取出来
        interrupt->PrintInt(val);  //执行内核的system call
        {//以下将Program counter+4，否則会一直执行instruction
            machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
            machine->WriteRegister(PCReg, machine->ReadRegister(PCReg) + 4);
            machine->WriteRegister(NextPCReg, machine->ReadRegister(PCReg)+4);
        }
    } else {
        printf("Unexpected user mode exception %d %d\n", which, type);
        interrupt->Halt();
        ASSERT(FALSE);
    }
}
