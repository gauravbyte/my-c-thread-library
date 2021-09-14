#ifndef _USER_LEVEL_THREAD_
#define _USER_LEVEL_THREAD_

#include <assert.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h> //for mmap function
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#define THREAD_STACK_SIZE (1024 * 64)

#define JOINREADY 1
#define JOINED 2
#define READY 1
#define RUNNING 2
#define TERMINATED 3
// SIGVTALRM is raised after every 50 msec
#define ALARM 50000


typedef unsigned long int mythread_t;
typedef unsigned int mythread_spinlock_t;
typedef unsigned int mythread_mutex_t;

typedef struct mythread {
  mythread_t tid;
  int state;   // JOINREADY, joined
  int status;  // ready, running,terminated,
  char *stack; // stack_base address
  void *(*start_routine)(void *);
  void *args;
  void *retval;
  sigjmp_buf context;
  int signal;
} mythread;

void fn(void);
void mythread_switch();
void start_timer(struct itimerval *timer);
void stop_timer(struct itimerval *timer);

void mythread_init(void);
int mythread_create(mythread_t *thread, void *(*start_routine)(void *),
                    void *args);
void mythread_exit(void *retval);
int mythread_join(mythread_t thread, void **retval);
int mythread_kill(mythread_t thread, int sig);
void mythread_cleanup(void);
void mythread_yield();

// temp function for debugging
void show1();

// spin lock
int mythread_spin_init(mythread_spinlock_t *lock);
int mythread_spin_lock(mythread_spinlock_t *lock);
int mythread_spin_trylock(mythread_spinlock_t *lock);
int mythread_spin_unlock(mythread_spinlock_t *lock);
typedef struct node {

  mythread *td;
  struct node *next;

} node;
double calculateTime();
typedef struct queue {

  struct node *head;
  struct node *tail;
  int count;

} queue;

void init_threads(queue *t);
void enqueue(queue *t, mythread *td);
mythread *dequeue(queue *t);
void show(queue *t);
mythread *get_node_by_tid(queue *t, mythread_t tid);

#endif