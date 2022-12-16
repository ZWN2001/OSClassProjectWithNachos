#define N_THREADS  10    // the number of threads
#define N_TICKS    1000  // the number of ticks to advance simulated time

#include "synch.h"
#include <stdio.h>
#include <time.h>
#include <iostream>

Semaphore *mutex,*barrier;
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
    MakeTicks(N_TICKS);

//while(control!=N_THREADS-1);
    printf("Thread %d rendezvous\n", which);

    mutex->P();
    count=count+1;
    mutex->V();
    if(count==N_THREADS)
    { barrier->V();
        printf("Thread %d is the last\n",which);
    }

    barrier->P();
    barrier->V();
    printf("Thread %d critical point\n", which);
}


void ThreadsBarrier()
{
    char s[N_THREADS][16];
    
    mutex = new Semaphore("mutex",1);
    barrier = new Semaphore("barrier",0);

    // Create and fork N_THREADS threads 

    for(int i = 0; i < N_THREADS; i++) {
	sprintf(s[i], "%d", i);
        threads[i] = new Thread(s[i]);
        mutex->P();
	    control = control+1;
        mutex->V();
        
        threads[i]->Fork(BarThread, i);
    }

}
