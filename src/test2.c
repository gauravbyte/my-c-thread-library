/*
Multiple threads: creates multiple threads and then waits for all of them to
complete. The threads should be run in FIFO order. There may be a large number
of these, too -
- which could stress whether you clean up the thread stack when done.
 */

#include "userthread.h"
#include <stdio.h>
#include <stdlib.h>

#define GREEN "\033[0;32;32m"
#define RED "\033[0;31;31m"
#define NONE "\033[m"

#define NUM_THREADS 10
int count[NUM_THREADS];
int msleep(unsigned int tms) { return usleep(tms * 1000); }
void *function() {
  unsigned long int threadid = mythread_gettid();
  printf("Hello from thread- tid - %lu \n", threadid);
  for (int i = 0; i < 10; i++) {
    usleep(10000);
    count[threadid - 1]++;
  }
  // sleep(1);
  printf("Ending threadtid - %lu \n", threadid);
}

int main(int argc, char *argv[]) {
  mythread_init();
  mythread_t p1[NUM_THREADS];

  for (int i = 0; i < NUM_THREADS; i++) {
    mythread_create(&p1[i], &function, NULL);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    mythread_join(p1[i], NULL);
  }
  int flag = 0;
  for (int i = 0; i < NUM_THREADS; i++) {
    if (count[i] != 10) {
      flag = 1;
    }
  }
  if (flag)
    printf(RED
           "\n**FAILED**: multiple thread test passed with %d threads\n" NONE,
           NUM_THREADS);
  else
    printf(GREEN
           "\n**PASSED**: multiple thread test passed with %d threads\n" NONE,
           NUM_THREADS);

  return 0;
}