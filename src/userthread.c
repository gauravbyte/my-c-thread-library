
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
int mythread_create(mythread_t *thread, void *(*start_routine)(void *),
                    void *args) {

  struct mythread *t;
  t = (mythread *)malloc(sizeof(mythread));

  t->tid = tid_count++;
  t->args = args;
  t->status = READY;
  t->start_routine = start_routine;
  t->signal = -1;
  t->state = JOINREADY;

  // allocate the memory for stack with mmap

  t->stack = mmap(NULL, THREAD_STACK_SIZE, PROT_READ | PROT_WRITE,
                  MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);

  if (t->stack == MAP_FAILED) {
    start_timer(&timer);
    printf("Insufficient res");
    return EAGAIN; // insufficient resources
  }


  int mythread_join(mythread_t thread, void **retval) {

  // find the node having that particular thread id

  mythread *temp;
  if (thread == td_cur->tid) {
    return EDEADLK;
  }
  temp = get_node_by_tid(threads, thread);
  if (temp == NULL) {
    return ESRCH;
  }

  // check if the thread is already joined or not
  if (temp->state == JOINED) {
    return EINVAL;
  }
  temp->state = JOINED;

  // othewise loop until the process is terminated
  while (1) {
    if (temp->status == TERMINATED) {
      break;
    }
  }
  // updating the retval  with the routine return value
  if (retval) {
    *retval = temp->retval;
  }
  return 0;
}
void mythread_exit(void *retval) {
  // changing the retval for the current running thread.
  td_cur->retval = retval;
  td_cur->status = TERMINATED;

  // raising the signal to call the mythread_switch
  raise(SIGVTALRM);
}
int mythread_kill(mythread_t thread, int sig) {
  // no signal
  if (sig == 0) {
    return 0;
  }
  // invalid signal
  if (sig < 0 || sig > 64) {
    return EINVAL;
  }

  // check if current thread id is same as the thread passed

  int status = -1;
  if (thread == td_cur->tid) {
    status = raise(sig);
  }

  // otherwise store it into the thread structure to raise it later
  else {
    mythread *temp = get_node_by_tid(threads, thread);
    if (temp != NULL) {
      temp->signal = sig;
      status = 0;
    } else {
      status = ESRCH;
    }
  }

  return status;
}


 

