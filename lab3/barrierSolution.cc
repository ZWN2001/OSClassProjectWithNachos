#define N_THREADS  10    // the number of threads
#define N_TICKS    1000  // the number of ticks to advance simulated time

#include <iostream>
#include "synch.h"
#include <stdio.h>
#include <time.h>
#include "system.h"
#include "syscall.h"


Semaphore *mutex,*barrier;
Thread *threads[N_THREADS];


int count = 0;

int control = 0;

int mark =0;

void MakeTicks(int n)  // advance n ticks of simulated time
{
//    sleep(1)
    for(int i = 0; i<n;i++){
        interrupt->OneTick();
    }
//   time_t start_t = time(NULL);
//
//   while(difftime(time(NULL), start_t)<N_TICKS);
   
}


void BarThread(_int which){
    MakeTicks(N_TICKS);
    printf("Thread %d rendezvous\n", which);

    mutex->P();
    count=count+1;
    if(count==N_THREADS)
    {
        printf("Thread %d is the last\n",which);
        barrier->V();
    }
    mutex->V();



    barrier->P();
    barrier->V();
    printf("Thread %d critical point\n", which);
}

void ThreadsBarrier(){
    char s[N_THREADS][16];

    mutex = new Semaphore("mutex",1);
    barrier = new Semaphore("barrier",0);

    // Create and fork N_THREADS threads

    for(int i = 0; i < N_THREADS; i++) {
        sprintf(s[i], "%d", i);
        threads[i] = new Thread(s[i]);
        threads[i]->Fork(BarThread, i);
    }

}
