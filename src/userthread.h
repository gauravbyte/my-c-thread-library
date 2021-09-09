#ifndef _USER_THREAD_H_
#define _USER_THREAD_H_

#include<setjmp.h>
#include<stdio.h>
#include<signal.h>
#include<sys/time.h>
#include<string.h>
#include<sys/mman.h>

/* 
myThread_create (thread,attr,start_routine,arg)
myThread_exit (status)
myThread_cancel (thread)
myThread_attr_init (attr)
myThread_attr_destroy (attr)
myThread_t pthread_self(void);3
myThread_yield(void)
myThread_join(int tid)
 */

typedef enum 
{
	running,
	ready,
	blocked,
	terminated,
}thread_state;

typedef enum
{
	waiting,readytojoin,joined
}joinstate;

typedef struct dthread
{
	int tid;
	joinstate state;
	sigjmp_buf context;//
	// Env Environment; //processor context area
	char *stack;	
	void *(start_routine)(void *);
	long long int sleep_time;		//time to sleep in microseconds
	//long previous_time;			//to remember last reduction time
	int wait_no;         			//no of semaphores for which thread is waiting
	int argc;
	char **argv; 					//arguments list passed to a thread
	void * args;   	 				//arguments passed to the thread;
	void * retval;					//return value stored in jmpbuf
	int signal;
}TCB;
void mythread_attr_init(void *args);
int mythread_create(unsigned long int *thread, void *(*start_routine) (void *), void *args);
void mythread_exit(void *retval);
int mythread_cancel(unsigned long int thread, int signal);
int mythread_pthread_self(void);
int dthread_join(unsigned long int thread, void **retval);
int dthread_kill(unsigned long int thread, int sig);
void dthread_cleanup(void);
void mythread_yeild(void );
int mythread_join(unsigned long int, void **returnvalues);
int mythread_switch();


#endif