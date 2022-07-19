/*
 * Program to perform 2d matrix multiplication by creating 3 threads.
 */

#include "userthread.h"
#include <stdio.h>
#include <stdlib.h>

#define GREEN "\033[0;32;32m"
#define RED "\033[0;31;31m"
#define NONE "\033[m"
// const double micro = 1.0e-6;
// double GLOBAL_PI = 0.0;
// double start_time=0, end_time=0,total_time=0;
long n = 10;
long matrix_size = 1000;
const double micro = 1.0e-6;
double GLOBAL_PI = 0.0;
double start_time = 0, end_time = 0, total_time = 0;
double calculateTime() {
  struct timeval TV;
  struct timezone TZ;
  int RC = gettimeofday(&TV, &TZ);
  if (RC == -1) {
    printf("Bad call to gettimeofday\n");
    return (-1);
  }
  return (((double)TV.tv_sec) + micro * ((double)TV.tv_usec));
}

typedef struct {
  int row_start;
  int row_end;
} rows;

// Matrix declaration

long int a[2000][2000];
long int b[2000][2000];
long int res[2000][2000];
long int correct_result[2000][2000];

// reading the values into 2d array
void random_matrix() {

  for (int i = 0; i < matrix_size; i++) {
    for (int j = 0; j < matrix_size; j++) {
      // fscanf(file, "%d", &(t->mat[i][j]));
    //   a[i][j] = rand() % 10;
    a[i][j]=3;
    }
  }
  for (int i = 0; i < matrix_size; i++) {
    for (int j = 0; j < matrix_size; j++) {
      // fscanf(file, "%d", &(t->mat[i][j]));
    //   b[i][j] = rand() % 10;
    b[i][j]=3;
    }
  }
}

void *partial_multiply(void *arg) {
  int sum;
  rows *temp = (rows *)arg;
  int start = temp->row_start;
  int end = temp->row_end + 1;
  // printf("D %d %d \n", start, end);
  for (int i = start; i < end; i++) {
    for (int j = 0; j < matrix_size; j++) {
      sum = 0;
      for (int k = 0; k < matrix_size; k++) {
        sum += a[i][k] * b[k][j];
      }
      res[i][j] = sum;
    }
  }
}

void check_result() {
  int incorrect = 0;
  for (int i = 0; i < matrix_size; i++) {
    for (int j = 0; j < matrix_size; j++) {

      if (res[i][j] != correct_result[i][j])
        incorrect = 1;

      // printf("%d\t",correct_result[i][j]);
    }
  }
  if (incorrect) {
    printf("matrix test failed");
  } else {
    printf(GREEN "\n**PASSED**: matrix test passed with k threads\n" NONE);
  }
}
int main(int argc, char *argv[]) {
  if (argc == 3) {
    n = strtol(argv[1], NULL, 10);
    matrix_size = strtol(argv[2], NULL, 10);
    // printf("%ld   n matrix %ld\n", n, matrix_size);
  }

  rows args[n];

  random_matrix(&a);
  random_matrix(&b);

//   int z = 0, d = 0, l = 0;
//   for (z = 0; z < matrix_size; z += 1) {
//     for (d = 0; d < matrix_size; d += 1) {
//       correct_result[z][d] = 0;
//       for (l = 0; l < matrix_size; l++) {
//         correct_result[z][d] = correct_result[z][d] + a[z][l] * b[l][d];
//       }
//       // printf("d = %d\n",d);
//     }
//     // printf("z =    %d\n",z);
//   }

  mythread_t tid[n];
  mythread_init();
  int partition_length = matrix_size / n;
  int i;
  if (n == 1) {
    args[0].row_start = 0;
    args[0].row_end = matrix_size - 1;
  } else {
    for (i = 0; i < n - 1; i++) {
      args[i].row_start = (i * partition_length);
      args[i].row_end = args[i].row_start + partition_length - 1;
    //   printf("%d   rowstart %d   end %d\n", i, args[i].row_start,
            //  args[i].row_end);
    }
    args[i].row_start = args[i - 1].row_end + 1;
    args[i].row_end = matrix_size - 1;
    // printf("%d   rowstart %d   end %d\n", i, args[i].row_start,
        //    args[i].row_end);
  }
  start_time = calculateTime();
  // creating three different threads
  for (int i = 0; i < n; i++) {
    mythread_create(&tid[i], partial_multiply, &args[i]);
  }

  for (int i = 0; i < n; i++) {
    mythread_join(tid[i], NULL);
  }
  end_time = calculateTime();
  //   check_result();
  total_time = end_time - start_time;
  printf("%ld      %lf", n, total_time);
  return 0;
}