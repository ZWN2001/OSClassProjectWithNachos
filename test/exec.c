#include "syscall.h" 


int 
main() 
{ 
int pid;
pid = Exec("../test/halt.noff"); 

Halt();
}
