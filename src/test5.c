/*  SpinLock test
*/



#include <stdio.h>

#include "userthread.h"

#define GREEN "\033[0;32;32m"
#define RED "\033[0;31;31m"
#define NONE "\033[m"


long r = 0, r1 = 0, r2 = 0, r3 = 0, r4 = 0,r5=0,r6=0, run = 1;
mythread_spinlock_t lock;
void *thread1(void *arg) {
	while(run == 1) {
		mythread_spin_lock(&lock);
        r++;
		mythread_spin_unlock(&lock);

		r1++;
    }
    return NULL;
}
void *thread2(void *arg) {
	while(run == 1) {
        mythread_spin_lock(&lock);
        mythread_yield();
        r++;
		mythread_spin_unlock(&lock);
		r2++;
    }
    return NULL;
}

void *thread3(void *arg) {
	while(run == 1) {
        mythread_spin_lock(&lock);
        mythread_yield();        
        r++;
		mythread_spin_unlock(&lock);
		r3++;
    }
    return NULL;
}
void *thread4(void *arg) {
	while(run == 1) {
        mythread_spin_lock(&lock);
        r++;
		mythread_spin_unlock(&lock);
		r4++;
    }
    return NULL;
}
void *thread5(void *arg) {
	while(run == 1) {
        mythread_spin_lock(&lock);
        r++;
		mythread_spin_unlock(&lock);
		r5++;
    }
    return NULL;
}
void *thread6(void *arg) {
	while(run == 1) {
        mythread_spin_lock(&lock);
        r++;
		mythread_spin_unlock(&lock);
		r6++;
    }
    return NULL;
}


int main() {
	mythread_t th1, th2, th3, th4,th5,th6; 
    mythread_init();
	mythread_spin_init(&lock);
    printf("\n\n");

	mythread_create(&th1, thread1, NULL);
	mythread_create(&th2, thread2, NULL);
	mythread_create(&th3, thread3, NULL);
	mythread_create(&th4, thread4, NULL);

	for(int i = 0; i < 100000000; i++);
	run = 0;
	mythread_join(th1, NULL);
	mythread_join(th2, NULL);
	mythread_join(th3, NULL);
	mythread_join(th4, NULL);


	if(r == r1 + r2 + r3 + r4) {
		printf(GREEN "**PASSED**: Spin lock test for race problem between 4 threads\n" NONE);
	}
	else {
		printf(RED "**FAILED**: Spin lock test for race problem between 4 threads\n" NONE);
	}

    return 0;
}
