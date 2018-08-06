/*
 * Example of DosSuspendThread/DosResumeThread usage.
 *
 * DosSuspendThread can be used to ensure mutual exclusion when a thread
 * knows by number all the other threads which might try to access the
 * shared resource.
 * 
 * In this example the main thread can call printf() freely because it
 * knows that the only other thread has been suspended, so the main
 * thread will not be interrupted, and because it knows that the
 * subthread must have been suspended while outside of a critical
 * section, and so the main thread will not be interrupting the
 * subthread's call to vio.
 * 
 * Any thread may suspend any other thread in its process, including the
 * main thread and itself.  If a thread suspends all the threads in a
 * process, including itself, then deadlock will result, and the process
 * will have to be killed externally.
 *
 * Note that there are three methods for managing critical sections
 * amongst threads in a process:
 *
 *	1. Semaphores - this is almost always the right solution
 *	2. DosEnterCritSec
 *	3. DosSuspendThread
 *
 * compile as: cl -Gs -AL -G2 -Lp suspend.c
 *
 * Created by Microsoft Corp. 1986
 */

#define INCL_DOSPROCESS
#define INCL_SUB

#include <os2def.h>
#include <bse.h>
#include "stdio.h"
#include "malloc.h"
#define STACK_SIZE	1024

extern void f_thread(void);
int flag;

void main()
{
	char *stkptr;
	TID thread_id;
	register int i;
	
	/* obtain pointer to the END of a block of memory */
	stkptr = (char *)malloc(STACK_SIZE) + STACK_SIZE - 1;
	
	/* create another thread */
	DosCreateThread(f_thread, &thread_id, stkptr);
	
	for(i = 0; i < 20; i++) {
		DosSuspendThread(thread_id);	/* suspend the subthread */
	
		printf("the main thread has suspended thread %u\n", thread_id);
		DosSleep(3000L);
	
		/* resume the subthread */
		printf("now resuming the suspended thread\n");
		DosResumeThread(thread_id);

		DosSleep(3000L);
	}
	DosExit(EXIT_PROCESS, 0);	   /* exit all threads */
}

void f_thread()
{
	while (1) {
		DosEnterCritSec();
		VioWrtTTY("subthread running\r\n", 19, 0);
		DosExitCritSec();
		DosSleep(300L);
	}
}
