/* 
--- MATRIX MULTIPLICATION ---

A simple application to demostrate the use of the Synchronization Barrier implementation.
Performs matrix-vector multiplication for now.

Future version: input user values: given an MxN matrix and an integer K, perform the Kth power of the input matrix.

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "matrix.h"
#include "barrier.h"

/* Method declarations */
void writeToResult(int key, int value, int pos);
int dotProduct(int rowVector[M_LEN], int colVector[M_LEN]);
int cleanMemory(int key);


/* Prints a vector (array) to console */
void printVector(int* vector, int size) {
  printf("<");
  for (int i=0; i<size-1; i++) {
    printf("%d ", vector[i]);
  }
  printf("%d>\n", vector[size-1]);
}


int main() {

  int KEY = 3000;     // key for Result in Shared Memory
  int vector[M_LEN] = {5, -1, 1}; 
  int matrix[M_LEN][M_LEN] = {{2,4,8},{7,-1,5},{9,3,-6}};
  
  pid_t child_pid, wpid;
  int status = 0;
  char semName[6] = "Semaf";
  char mutexName[7] = "SemafB";

	//CREATE SHARED MEMORY FOR RESULT
  int shmid1;
  if ( (shmid1 = shmget(KEY, sizeof(resultVector), IPC_CREAT | S_IRWXU)) < 0 ) {
    perror("Error while creating shmget");
    return 1;
  }

  resultVector *rv = (resultVector*) shmat(shmid1, 0, 0);
  if (rv == (void *) -1) {
    perror("Error while creating shmat1");
    return 1;
  }

  rv->vector[0]= 0;
  rv->vector[1]= 0;
  rv->vector[2]= 0;

  printf("Input Matrix:\n");
  for(int r; r<M_LEN; r++) {
    printVector(&matrix[r][0], M_LEN);
  }
  putchar('\n');
  printf("Input Vector:\n");
  printVector(&vector[0], M_LEN);
  putchar('\n');

  printf("\nProcesses to start: %d\n", M_LEN);
	//Initialize barrier using a struct called Barrier with name of mutex, semaphore, size and key of shared memory 
  Barrier* barr = malloc(sizeof(Barrier));
  strcpy(barr->semName, semName);
	strcpy(barr->mutexName, mutexName);
  barr->size = M_LEN;
  barr->shmKey = 2000;
  initBarrier(barr);

  for (int id=0; id<M_LEN; id++) {
    if ((child_pid = fork()) == 0) {
        //printf("Row %d\n", id);
        int val = dotProduct(matrix[id], vector);
        writeToResult(KEY, val, id);
        printf("Process %d reached the barrier\n", id);
        waitBarrier(barr); // espera al resto
        printf("We can continue! \n");
        exit(0);
    }
  }
  
  while ((wpid = wait(&status)) > 0);
  //printf("All processes finished, we are in parent\n");
  printf("Result Vector: ");
  printVector(&rv->vector[0], M_LEN);
  destroyBarrier(barr);
  cleanMemory(KEY);
  return 0; 
}

//CALCULATE VALUE TO WRITE
int dotProduct(int rowVector[M_LEN], int colVector[M_LEN]){
  int sum=0;
  for (int x=0; x<M_LEN; x++) {
    sum += rowVector[x] * colVector[x];
  }
	return sum;
}

//WRITE IN SHARED MEMORY
void writeToResult(int key, int value, int pos){
	//Open shared memory
	int shmid1;
  if ((shmid1 = shmget(key, sizeof(resultVector), IPC_CREAT | S_IRWXU)) < 0 ) {
    perror("Error while using shmget in writeToResult\n");
    return;
  }
  resultVector  *rv = (resultVector*) shmat(shmid1, 0, 0);
  if (rv == (void *) -1) {
    perror("Error while using shmat in writeToResult\n");
    return;
  }
  //WRITE AND NOTIFY
  rv->vector[pos] = value;
  printf("Process %d finished writing\n", pos);

}

//CLEAN SHARED MEMORY OF resultVector
int cleanMemory(int key){
  printf("Cleaning memory...\n");
  int shmid;
  if (( shmid = shmget(key,  sizeof(resultVector), S_IRWXU)) < 0 ) {
    perror("Error in shmget while cleaning");
    return 1;
  }
  struct shmid_ds ds;
  if (shmctl(shmid, IPC_RMID, &ds) < 0) {
    perror("Error while cleaning shared memory");
    return 1;
  }
	printf("Successful cleaning\n");
  return 0;
}
