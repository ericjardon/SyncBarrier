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

#define M_LEN 3


/* Method declarations */
void writeToResultMatrix(int key, int value, int i, int j);

int rowColumnProd(int matrix[100][100], int* rowVector, int size, int j);

int cleanMemory(int key);


/* Prints a vector (array) horizontally to console */
void printVector(int* vector, int size) {
  printf("<");
  for (int i=0; i<size-1; i++) {
    printf("%d ", vector[i]);
  }
  printf("%d>\n", vector[size-1]);
}


int main() {
  int i, j, N, k;
  int K=1;

  // open shared memory
  int KEY1 = 3000;
  int shmid1;

  if ( (shmid1 = shmget(KEY1, sizeof(ShMatrix), IPC_CREAT | S_IRWXU)) < 0 ) {
    perror("Error while creating shmget");
    return 1;
  }

  // associate shm to variable
  ShMatrix *shm = (ShMatrix*) shmat(shmid1, 0, 0);

  // sanity checks
  if (shm == (void *) -1) {
    perror("Error while creating shmat1");
    return 1;
  }

  printf("Enter the size of square Matrix (between 1 and 100): ");
  scanf("%d", &N);

  shm->N = N;

  printf("Enter the elements of the matrix:\n");

  int val;
  for (i=0; i<N; i++) {
    for (j=0; j<N; j++) {
      scanf("%d", &val);
      shm->M[i][j] = val;
      shm->AUX[i][j] = val;
    }
  }

  printf("PRINTING INPUT MATRIX\n");
  for (i=0; i<N; i++) {
    printVector(&(shm->M[i][0]), N);
  }

  printf("Enter the desired power (1-10)\n");
  scanf("%d", &K);
  printf("Elevate to the %d th power\n", K);

  /* INITIATE MATRIX MULTIPLICATION */
  pid_t child_pid, wpid;
  int status = 0;

  printf("\nProcesses to start: %d\n", N);

  /* Synchronization Barrier setup */

  Barrier* barriers[K-1];  // for the k-th power we need k-1 operations

  for (k = 0; k < K-1; k++) {
    Barrier* barr = malloc(sizeof(Barrier));
    barr->size = N;
    barr->shmKey = 2000 + k;

    barriers[k] = barr;

    initBarrier(barriers[k]);    // initialize shared memory and semaphores for the k-th barrier
    printf("Initialized Barrier with names:\n");
    puts(barriers[k]->semName);
    puts(barriers[k]->mutexName);
  }

  // Fork N processes
  for (i = 0; i < N; i++) {
    // For every child process
    if ((child_pid = fork()) == 0) {

        // For every k-th power
        for (k = 0; k < K-1; k++) {
          // Make a temporary copy of the i-th row of M
          int rowCopy[N];
          for (j = 0; j < N; j++) rowCopy[j] = shm->M[i][j];

          // Compute the product of the i-th row of M with every column of AUX
          for (j=0; j < N; j ++) {
            int val = rowColumnProd(shm->AUX, &rowCopy[0], N, j);
            
            writeToResultMatrix(KEY1, val, i, j); // row i * col j is written to M[i][j]
          }
          
          waitBarrier(barriers[k]); // waits for the remaining processes
          printf("Process %d continue\n", i);
        }
        printf("Process %d COMPLETED\n", i);
        exit(0);
    }
  }

  /* Parent process waits for all children to exit */
  while ((wpid = wait(&status)) > 0);

  putchar('\n');
  printf("RESULTING MATRIX: \n");

  for (i=0; i<N; i++) {
    printVector(&(shm->M[i][0]), N);
  }

  putchar('\n');

  for(k = 0; k < K-1; k++) {
    destroyBarrier(barriers[k]);
  }

  cleanMemory(KEY1);
  return 0;
}


// COMPUTES DOT PRODUCT BETWEEN FIXED ROW AND COLUMN VECTOR AT GIVEN INDEX
int rowColumnProd(int matrix[100][100], int* rowVector, int size, int j){

  int sum=0;
  
  for (int x=0; x<size; x++) {
    sum += rowVector[x] * matrix[x][j];
  }
	
  return sum;
}

// WRITE RESULT TO SHARED MEMORY
void writeToResultMatrix(int key, int value, int i, int j){
	//Open shared memory
	int shmid1;
  if ((shmid1 = shmget(key, sizeof(ShMatrix), IPC_CREAT | S_IRWXU)) < 0 ) {
    perror("Error while using shmget in writeToResult\n");
    return;
  }

  // Associate to variable
  ShMatrix  *shm = (ShMatrix*) shmat(shmid1, 0, 0);

  // sanity check
  if (shm == (void *) -1) {
    perror("Error while using shmat in writeToResult\n");
    return;
  }
  
  
  shm->M[i][j] = value;
}


//CLEANS SHARED MEMORY
int cleanMemory(int key){
  printf("Cleaning memory...\n");
  int shmid;
  if (( shmid = shmget(key,  sizeof(ShMatrix), S_IRWXU)) < 0 ) {
    perror("Error in shmget while cleaning");
    return 1;
  }
  struct shmid_ds ds;
  if (shmctl(shmid, IPC_RMID, &ds) < 0) {
    perror("Error while cleaning shared memory");
    return 1;
  }
	printf("Successfully cleaned Shared Memory\n");
  return 0;
}
