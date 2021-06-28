#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h> 

#include "barrier.h"
#define NUM_PROC 10

/* 
--- Testing Program for Synchronization Barriers ---

This program emulates concurrent work divided amon NUM_PROC processes,
to demostrate funcionality of our barrier implementation.

They may all finish at different times but must wait for all 
of them to get to the synchronization barrier.

Random waiting times are assigned to a process to emulate "work".

*/

void randwait(int k) {
  int x = (int) (k*drand48()+ 1);
  usleep(x);
}


int main() {
  
  Barrier * barr = (Barrier*) malloc(sizeof(Barrier));
  char semName[6] = "BarSe";
  char mutexName[7] = "BarMut"; 
  
  // Set barrier attributes
  strcpy(barr->semName, semName);
  barr->size = NUM_PROC;
  barr->shmKey = 2000;
  strcpy(barr->mutexName,mutexName);
  initBarrier(barr);
  

  pid_t child_pid, wpid;
  int status = 0;


  for (int id=0; id<NUM_PROC; id++) {

    // Fork NUM_PROC child processes
    if ((child_pid = fork()) == 0) {
        
        printf("Process #%d begins\n", id);
        randwait(50); // run for some amount of time

        // reaches barrier
        printf("Process %d reached the barrier\n", id);
        waitBarrier(barr);

        // Barrier finishes waiting 
        printf("I am freed by the barrier \n");
        exit(0);
    }

  }
  
  while ((wpid = wait(&status)) > 0);
  printf("All processes ended\n");
  
  destroyBarrier(barr);
  return 0;
}