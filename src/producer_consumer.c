#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "userthread.h"
#include <semaphore.h>
#define THREAD_NUM 10

mythread_spinlock_t lock;
long  CONTAINER = 2000;
long NUM_THREADS = 200;
int buffer[20000];
int count = 0;
int actioncount = 0;

void *producer(void *args) {
  unsigned long int tid = mythread_gettid();
  for (int i = 0; i < CONTAINER * 10; i++) {
    // Produce
    // srand(time(NULL));
    while (count > CONTAINER - 1)
      ;
    int x = rand() % 100;

    // Add to the buffer
    if (rand() % 2)
      mythread_yield();
    mythread_spin_lock(&lock);
    buffer[count] = x;
    count++;
    mythread_spin_unlock(&lock);
    printf("buffer = %d added %d\n", count, x);
    actioncount++;
    //  usleep(100);
    if (actioncount > CONTAINER * 10)
      return;
  }
}

void *consumer(void *args) {

  unsigned long int tid = mythread_gettid();
  int y;
  for (int i = 0; i < CONTAINER * 10; i++) {
    while (count <= 0)
      ;
    // Remove from the buffer
    if (rand() % 2)
      mythread_yield();
    mythread_spin_lock(&lock);
    y = buffer[count - 1];
    count--;
    mythread_spin_unlock(&lock);

    // Consume
    printf("buffer = %d consumed %d\n", count, y);
    actioncount++;
    //  usleep(100);
    if (actioncount > CONTAINER * 10)
      return;
  }
}

int main(int argc, char *argv[]) {

  // srand(time(NULL));
 if (argc == 3) {
    NUM_THREADS = strtol(argv[1], NULL, 10);
    CONTAINER = strtol(argv[2], NULL, 10);

  }


  mythread_t th[THREAD_NUM];
  mythread_spin_init(&lock);
  mythread_init();

  mythread_spin_init(&lock);
  int i;
  for (i = 0; i < THREAD_NUM; i++) {
    // if(i%2==0)
    mythread_create(&th[i], &producer, NULL);

    // else
    mythread_create(&th[i], &consumer, NULL);
  }
  for (i = 0; i < THREAD_NUM; i++) {
    if (mythread_join(th[i], NULL) != 0) {
      perror("Failed to join thread");
    }
  }

  return 0;
}