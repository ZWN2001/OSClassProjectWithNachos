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


void ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);
    int val;
    int status, exit, threadID, programID;
    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");
    switch (which) {
        case SyscallException:
            switch(type) {
                case SC_PrintInt:
                    DEBUG(dbgSys, "PrintInt\n"); // 使用Debug mode
                    val = kernel->machine->ReadRegister(4); //将MIPS machine存的参数取出来
                    interrupt->PrintInt(n);
//                    kernel->interrupt->PrintInt(n);      //执行内核的system call，
                    {//以下将Program counter+4，否則会一直执行instruction
                        // set previous programm counter (debugging only)
                        kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                        // set programm counter to next instruction (all Instructions are 4 byte wide)
                        kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                        // set next programm counter for brach execution
                        kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
                    }
                    return;

                case SC_Halt:
                    DEBUG('a', "Shutdown, initiated by user program.\n");
                    interrupt->Halt();

                case SC_Exit:
                    DEBUG(dbgAddr, "Program exit\n");
                    val=kernel->machine->ReadRegister(4);
                    cout << "return value:" << val << endl;
                    kernel->currentThread->Finish();
                    break;

                case SC_Exec:
                    int FilePathMaxLen = 100;
                    char filepath[FilePathMaxLen];
                    int fileaddr = machine->ReadRegister(4);
                    for (i = 0; filepath[i] != '\0'; i++){
                        if(!machine->ReadMem(fileaddr+4*i,4,(int*)(filepath+4*i)))
                            break;
                    }
//		            DEBUG('a',"Thread %d Exec: file: %s\n", which, filepath);
                    printf("Thread %d Exec: file: %s\n", which, filepath);
                    extern void StartProcess(char *filename);
                    StartProcess(filepath);
                default:
                    cerr << "Unexpected system call " << type << "\n";
                    break;
            }
            break;
        default:
            cerr << "Unexpected user mode exception " << (int)which << "\n";
            break;
    }
    ASSERTNOTREACHED();
}
