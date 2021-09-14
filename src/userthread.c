#include "userthread.h"

/* variables that can be accessed from all threads */

static queue *threads;           // list of all the threads
static mythread *td_cur;         // detials of thread which is running
struct itimerval timer;          // for gettimer and settimer function input
static mythread_t tid_count = 0; // to assign thread id values

/*
*   glibc uses ptrmangle function to jmpbuf values for newer gcc
    so ptrmangle function helps to
    Because there is one herePointer encryption Features!
     GlibcPTR_MANGLE ! The value in the jmp_buf dumped
    is actually an over-valued value, not the original value.
    ptrmangle function helps us to get around that
*   https://programmersought.com/article/41611348414/
*/

static long int PTR_MANGLE(long int p) {
  long int ret;
  // assembly code
  asm(" mov %1, %%rax;\n"
      " xor %%fs:0x30, %%rax;"
      " rol $0x11, %%rax;"
      " mov %%rax, %0;"
      : "=r"(ret)
      : "r"(p)
      : "%rax");
  return ret;
}

static long int PTR_DEMANGLE(long int var) {
  asm("ror $0x11, %0\n"
      "xor %%fs:0x30, %0"
      : "=r"(var)
      : "0"(var));
  return var;
}

/*
 *   Init function: before using any of the other methods user has to call this
 * method It initializes the global threads list and signal. Initializes the
 * main thread details Timer is started for raising sigvtalrm signal which
 * further will call mythread_switch
 */

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

/*
 *   This method is used to clean all the threads before exiting
 */

void mythread_cleanup() {
  mythread *td;
  int count = threads->count;
  for (int i = 0; i < count; i++) {
    td = dequeue(threads);
    free(td);
  }
  // final threads remove
  free(threads);
}

void mythread_yield() {
  // mythread_switch();
  raise(SIGVTALRM);
}
/*
 *   Wrapper function to run different thread routines based on current thread
 * details
 */

void fn(void) {
  td_cur->retval = td_cur->start_routine(td_cur->args);
  td_cur->status = TERMINATED;
  raise(SIGVTALRM);
  return;
}

/*
 *   Whenever sigvtalrm is raised mythread_switch is called,
 *   here current running thread thread is enqueued,
 *   and is replaced by new thread details, which is not terminated
 *   If no such valid threads found in the queue, program exits
 *   Before jumping to the thread context,
 *       we check if the thread has any signals to be raised
 */

void mythread_switch(int sig) {
  stop_timer(&timer);
  mythread_t temp_tid = td_cur->tid;
  if (sigsetjmp(td_cur->context, 1) == 1) {
    return;
  }

  enqueue(threads, td_cur);
  if (td_cur->status == RUNNING) {
    td_cur->status = READY;
  }

  // getting the next thread details:
  // in loop, to check if the thread is already terminated or not
  mythread *temp;
  int threads_count = threads->count;
  for (int i = 0; i < threads_count; i++) {
    if (td_cur->status == RUNNING) {
      td_cur->status = READY;
    }

    temp = dequeue(threads);
    if (temp->status == READY) {
      td_cur = temp;
      td_cur->status = RUNNING;
      break;
    } else if (temp->status == TERMINATED) {
      enqueue(threads, temp);
    }
  }
  if (td_cur->tid == temp_tid) {
    exit(0);
  }
  if (td_cur->signal != -1) {
    raise(td_cur->signal);
    td_cur->signal = -1;
  }

  start_timer(&timer);
  siglongjmp(td_cur->context, 1);
}

/*
 *   Starting the timer
 */

void start_timer(struct itimerval *timer) {
  timer->it_value.tv_sec = 0;
  timer->it_value.tv_usec = ALARM;
  timer->it_interval.tv_sec = 0;
  timer->it_interval.tv_usec = ALARM;
  setitimer(ITIMER_VIRTUAL, timer, 0);
}

/*
 *   Stopping the timer, which means now no more sigvtalrm is raised
 */

void stop_timer(struct itimerval *timer) {
  timer->it_value.tv_sec = 0;
  timer->it_value.tv_usec = 0;
  timer->it_interval.tv_sec = 0;
  timer->it_interval.tv_usec = 0;
  setitimer(ITIMER_VIRTUAL, timer, 0);
}

/*
 *   For thread stack allocation mmap() is used
 *   mythread_t thread value is changed with respective tid for user side
 *   Saving the threads context
 *   Changing the stack,base pointer and program counter for that thread
 *   Adding it to the threads list
 */

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

  // assigning the thread with the tid
  *thread = t->tid;

  // saving the context and changing the stack pointer, base pointer and program
  // counter
  sigsetjmp(t->context, 1);
  t->context[0].__jmpbuf[6] =
      PTR_MANGLE((long int)t->stack + THREAD_STACK_SIZE - sizeof(long int));
  t->context[0].__jmpbuf[1] =
      PTR_MANGLE((long int)t->stack + THREAD_STACK_SIZE - sizeof(long int));
  t->context[0].__jmpbuf[7] = PTR_MANGLE((long int)fn);
  // printf("%p", t->context);

  enqueue(threads, t);

  return 0;
}

/*
 *   All error checks are done before waiting for the thread specified to
 * terminate while() with some condition is used for waiting purpose retval
 * passed as argument is modified by the actual retval of the thread
 */

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

/*
 *   Status is changed and retval is updated, also sigvtalrm is raised to get
 * the new thread
 */

void mythread_exit(void *retval) {
  // changing the retval for the current running thread.
  td_cur->retval = retval;
  td_cur->status = TERMINATED;

  // raising the signal to call the mythread_switch
  raise(SIGVTALRM);
}

/*
 *   error checking is done
 *   see if it the thread id passed is of the current running thread
 *   if yes then raise signal, otherwise append it to the threads structure, to
 * raise it later
 */

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

/*
 *   Spin lock is implemeted with the help of __sync_lock_test_and_set
 *   and __sync_lock_release methods
 */

int mythread_spin_init(mythread_spinlock_t *lock) {
  *lock = 0;
  return 0;
}
int mythread_spin_lock(mythread_spinlock_t *lock) {
  while (__sync_lock_test_and_set(lock, 1))
    ;
  return 0;
}
// returning EBUSY when lock is being held by other thread
int mythread_spin_trylock(mythread_spinlock_t *lock) {
  if (__sync_lock_test_and_set(lock, 1)) {
    return EBUSY;
  } else {
    return 0;
  }
}
int mythread_spin_unlock(mythread_spinlock_t *lock) {
  if (*lock == 1) {
    __sync_lock_release(lock, 0);
    return 0;
  }
  return EINVAL;
}

void show1() { show(threads); }

void init_threads(queue *t) {
  t->head = NULL;
  t->tail = NULL;
  t->count = 0;
}

void enqueue(queue *t, mythread *td) {
  node *ptr = (node *)malloc(sizeof(node));
  // assigning the new node details
  ptr->td = td;
  ptr->next = NULL;
  // checking if the queue is empty, if empty head and tail points to ptr
  if (t->count == 0) {
    t->head = t->tail = ptr;
  }

  // otherwise append the node to last changing the tail pointer to ptr
  else {
    t->tail->next = ptr;
    t->tail = ptr;
  }
  // increment the count
  t->count = t->count + 1;
  return;
}

mythread *dequeue(queue *t) {
  // check if the queue is empty
  if (t->count == 0) {
    return NULL;
  }
  mythread *td = t->head->td;
  t->head = t->head->next;
  t->count = t->count - 1;
  // check if the new head is null or not , if yes then change the tail
  if (t->head == NULL) {
    t->tail = NULL;
  }
  return (td);
}

void show(queue *t) {
  node *ptr;
  if (t->head == NULL) {
    printf("\nEmpty");
    return;
  }
  ptr = t->head;
  while (ptr != NULL) {
    printf("\nTID: %ld", ptr->td->tid);
    ptr = ptr->next;
  }
  printf("\nTOTAL: %d", t->count);
}


mythread *get_node_by_tid(queue *t, mythread_t tid) {
  node *temp;
  mythread *td;
  if (t->head == NULL) {
    return NULL;
  }
  temp = t->head;
  while (temp != NULL) {
    if (tid == temp->td->tid) {
      return temp->td;
    } else {
      temp = temp->next;
    }
  }
  return NULL;
}