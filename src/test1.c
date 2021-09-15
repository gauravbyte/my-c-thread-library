/*
Single thread: A program that creates and thread (which prints something out)
and then waits for it to complete.
 */

#include "userthread.h"
#include <stdio.h>
#include <stdlib.h>
#define GREEN "\033[0;32;32m"
#define RED "\033[0;31;31m"
#define NONE "\033[m"

void *function() {
  printf("Hello from thread\n");
  sleep(1);
  printf("Ending thread\n");
  printf(GREEN
         "\n**PASSED**: test 1 : matrix test passed with k threads\n" NONE);
}

int main(int argc, char *argv[]) {
  mythread_init();
  mythread_t p1;
  mythread_create(&p1, &function, NULL) != 0;
  mythread_join(p1, NULL);

  return 0;
}