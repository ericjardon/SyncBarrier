#ifndef BARRIER_H
#define BARRIER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>    


/* BARRIER STRUCT DEFINITION */
typedef struct Barrier {
  int size;                 // num of processes
  int shmKey;               // uniquely identifies shared mem
  char semName[20];         // for named counting semaphore
  char mutexName[21];       // for named binary semaphore
} Barrier;

/* Shared memory Struct */
typedef struct sharedCount {
  int count; 
} sharedCount;

/* METHOD DECLARATIONS */
int waitBarrier(Barrier* barrier);

int initBarrier(Barrier* barrier);

int destroyBarrier(Barrier* barrier); 


#endif