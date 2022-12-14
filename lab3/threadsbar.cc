#define N_THREADS  10    // the number of threads
#define N_TICKS    0.1  // the number of ticks to advance simulated time

#include "synch.h"
#include <stdio.h>
#include <time.h>
#include <iostream>
in

Semaphore *waiter,*accepter;
Thread *threads[N_THREADS];




int count = 0;

int control = 0;

int mark =0;

void MakeTicks()  // advance n ticks of simulated time
{
   time_t start_t = time(NULL);
   
   while(difftime(time(NULL), start_t)<N_TICKS);
   
}


void BarThread(_int which)
{   
//    default_random_engine e;
 
//      int n = e();
        // MakeTicks();

while(control!=N_THREADS-1);
    printf("Thread %d rendezvous\n", which);
    
    waiter->P();
    count=count+1;
    if(count==N_THREADS)    
     { accepter->V();
       printf("Thread %d is the last\n",which);}

    waiter->V();
  
   
    accepter->P();
    accepter->V();
    printf("Thread %d critical point\n", which);
}


void ThreadsBarrier()
{
    char s[N_THREADS][16];
    
    waiter = new Semaphore("waiter",1);
    accepter = new Semaphore("accepter",0);

    // Create and fork N_THREADS threads 

    for(int i = 0; i < N_THREADS; i++) {
	sprintf(s[i], "%d", i);
        threads[i] = new Thread(s[i]);
    waiter->P();
	control = control+1;
	waiter->V();
        
        threads[i]->Fork(BarThread, i);
    }

}
