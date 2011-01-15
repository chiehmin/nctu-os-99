#include <assert.h>
#include <ucontext.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
// thread structure
typedef struct th {
   ucontext_t uct; 
   int tid;//thread ID
   int time_slice; // time quantum for preemptive scheduling 
   int state;// thread state 
   int priority; 
   char stack[16384];
   void *u;// your defined structure to hold stack, and other user defined area

} th_t;

extern int th_init(th_t *thread);//th_t initialization, you should allocate stack space here
extern int setpreemptive();// the scheduler will be preemptive
extern int setnonpreemptive();// the scheduler will be nonpreemptive


// thread creation
extern int th_fork(th_t *thread, void *(*start_routine)(void*), void *arg);
// thread joining, wait for thread termination
extern int th_wait(th_t *thread, void *status);
// thread termination
extern void th_exit(int status);
// thread canceled by another thread
extern int th_kill(th_t *thread);
// thread relinquish use of the processor
// and wait in the ready queuee
extern void th_yield();
