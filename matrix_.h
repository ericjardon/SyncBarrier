#ifndef SHMATRIX_H
#define SHMATRIX_H

typedef struct ShMatrix {
  int N;
  int M[100][100];          // the product accumulated matrix
  int AUX[100][100];        // the input matrix
} ShMatrix;

#endif