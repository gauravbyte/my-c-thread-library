/* Multiple threads + lock: creates threads and has them call lock/unlock on a
single lock around a critical section.
 */

#include <stdio.h>

#include "userthread.h"

void *foo() {
  for (int i = 0; i < 5; i++) {
    mythread_yield();
    printf("foo - %d\n", i);
    usleep(200);
  }
}
void *bar() {
  for (int i = 0; i < 5; i++) {
    printf("bar - %d\n", i);
    usleep(200);
  }
}
void *tank() {
  for (int i = 0; i < 5; i++) {
    if (i % 2 == 0) {
      mythread_yield(); // yeilds for even counts
    }
    printf("tank - %d\n", i);
    usleep(200);
  }
}

int main() {
  mythread_t t1, t2, t3;
  int i, j, k;
  mythread_init();
  mythread_create(&t1, &foo, NULL);
  mythread_create(&t2, &bar, NULL);
  mythread_create(&t3, &tank, NULL);

  mythread_join(t1, NULL);
  mythread_join(t2, NULL);
  mythread_join(t3, NULL);
}