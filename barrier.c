#include "barrier.h"

#define ABC "abcdefghijklmnopqrstuvwxyz1234567890"
#define ABC_LEN 36
#define NAME_LEN 5

/*
ORIGINAL DEVELOPMENT TEAM
- Eric Andrés Jardón Chao (ericjardon)
- Georgina Alejandra Gámez Melgar (GinaJame)
- Bryan Alexis Monroy Álvarez (AlexisBMA)
- Einar Lopez Altamirano (EinarLop)

Made as the final research project for the TC2025 Advanced Programming course at ITESM.
Enhanced and optimized in Jun 2021

*/

int giveRandomNames(Barrier* barrier);

int initBarrier(Barrier *barrier){
    /* 
    Initializes a Synchronization Barrier and necessary data structures.
    Params: a Barrier struct with user values set: size, shmKey, semName and mutexName.
    */

  printf("\tInitializing Sync Barrier of size %d\n", barrier->size);

  giveRandomNames(barrier);

  sem_unlink(barrier->semName);
  sem_unlink(barrier->mutexName);

  int KEY = barrier->shmKey;    // to identify unique memory allocation
  sem_t *sem;                   // to block waiting processes
  sem_t *mutex;                 // to protect the counter variable

  
  // Allocate shared memory and initialize values (count).
  int shmid1;
  if ( (shmid1 = shmget(KEY, sizeof(sharedCount), IPC_CREAT | S_IRWXU)) < 0 ) {
    perror("Error in shmget while creating barrier");
    return 1;
  }

  sharedCount *cmp = (sharedCount*) shmat(shmid1, 0, 0);
  if (cmp == (void *) -1) {
    perror("Error in shmat while creating barrier");
    return 1;
  }

  cmp->count = barrier->size;
   
  // Initialize counting semaphore
  sem = sem_open(barrier->semName, O_CREAT, S_IRWXU, 0);
  if (sem==SEM_FAILED) {
    printf("\tSemaphore failed\n");
    exit(EXIT_FAILURE);
  }

  // Initialize binary semaphore
  mutex = sem_open(barrier->mutexName, O_CREAT, S_IRWXU, 1);
  if (mutex ==SEM_FAILED) {
    printf("\tMutex failed\n");
    exit(EXIT_FAILURE);
  } 
  
  sem_close(sem);
  sem_close(mutex);
	return 0;
}


int waitBarrier(Barrier* barrier){
    /* 
    Decrements the barrier counter (number of processes left to reach the barrier) and verifies:
    If the count is greater than zero, it makes the calling process wait.
    If the count is exactly zero, all the waiting processes are freed.

    Params: a Barrier struct with user values set: size, shmKey, semName and mutexName.
    */

  sem_t *sem;
  sem_t *mutex;

  sem = sem_open(barrier->semName, 0);
  mutex = sem_open(barrier->mutexName, 0);
  int KEY = barrier->shmKey;
  
  // Open shared memory with given key
  int shmid1;
  if ( (shmid1 = shmget(KEY, sizeof(sharedCount), IPC_CREAT | S_IRWXU)) < 0 ) {
    perror("Error in shmget in waitBarrier");
    return 1;
  }
  
  sharedCount *cmp = (sharedCount*) shmat(shmid1, 0, 0);

  if (cmp == (void *) -1) {
    perror("Error in shmat in waitBarrier");
    return 1;
  }
  
  // Decrement the counter
  sem_wait(mutex);
  (cmp->count)--;
  sem_post(mutex);

  printf("\t Barrier waiting for %d processes\n", cmp->count);

  if(cmp->count==0){
    for(int i =0;i<barrier->size;i++){
		sem_post(sem);      // Free every waiting process
  	}  
  
  } else {
    // Calling process is made to wait
    sem_wait(sem);
  }

  sem_close(sem);
  sem_close(mutex);
  return 0;
}


int destroyBarrier(Barrier* barrier){
    /* 
    Wipes out shared memory and destroys auxiliary semaphores.

    Params: a Barrier struct with user values set: size, shmKey, semName and mutexName.
    */

  int KEY = barrier->shmKey; 
  int shmid;
  if (( shmid = shmget(KEY,  sizeof(sharedCount), S_IRWXU)) < 0 ) {
    perror("Error in shmget at destroyBarrier");
    return 1;
  }

  struct shmid_ds ds;
  
  if (shmctl(shmid, IPC_RMID, &ds) < 0) {
    perror("Error in shmctl at destroyBarrier");
    return 1;
  }

  sem_unlink(barrier->semName);
  sem_unlink(barrier->mutexName);
	// printf("Barrier succesfully destroyed\n");
	return 0;
}


int giveRandomNames(Barrier* barrier) {
    char semName[NAME_LEN+1];
    char mutexName[NAME_LEN+1];

    memset(semName,'\0',NAME_LEN+1);
    memset(mutexName,'\0',NAME_LEN+2);

    const char letters[] = ABC;
    int c;
    
    for (c=0; c<NAME_LEN; c++) {
      int key = rand() % ABC_LEN;
      int keyb = (key + 1) % ABC_LEN;
      semName[c] = letters[key];
      mutexName[c] = letters[keyb];
    }

    strcpy(barrier->semName, semName);
    strcpy(barrier->mutexName, mutexName);

    return 0;
}