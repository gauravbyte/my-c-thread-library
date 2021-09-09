#ifndef _USER_THREAD_H_
#DEFINE _USER_THREAD_H_

#include<setjmp.h>
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


typedef struct mythread
{
	int tid;
	status state;
	sigjmp_buf jb;
	char stack[STACKSIZE];
	struct semaphore_struct * th_semaphore;

	long sleep_time;						//in microseconds
	//long previous_time;						//to remember last reduction time
	int wait_no;							//no of semaphores for which thread is waiting

	fun_type start_function;
	void * argument;
	void * ret_val;
}TCB;