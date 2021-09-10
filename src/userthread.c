
#include "userthread.h"
#include "fifo.h"

void mythread_init(void) {
  threads = (queue *)malloc(sizeof(queue));
  td_cur = (mythread *)malloc(sizeof(mythread));
  init_threads(threads);

  // initialising the current thread with main thread details
  td_cur->tid = tid_count++;
  td_cur->status = RUNNING;
  td_cur->state = JOINREADY;
  td_cur->start_routine = NULL;
  td_cur->args = NULL;
  td_cur->retval = NULL;
  td_cur->signal = -1;
  sigsetjmp(td_cur->context, 1);

  // initialising signal for timer
  struct sigaction sa;
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGVTALRM);
  // whenever sigvtalrm is raised, mythread_switch is called
  sa.sa_handler = &mythread_switch;
  sa.sa_flags = 0;
  sa.sa_mask = mask;
  sigaction(SIGVTALRM, &sa, NULL);
  atexit(mythread_cleanup);
  start_timer(&timer);
}
mythread_t mythread_gettid() { return td_cur->tid; }
