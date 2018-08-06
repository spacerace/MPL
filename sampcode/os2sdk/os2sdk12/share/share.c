/*
 * This example illustrates the use of shared memory and ram semaphores
 * between separate processes.  One process reads and stores keystrokes
 * into a circular buffer in shared memory, and another reads the buffer
 * and writes to standard output. The output program throttles output to
 * one character per sec so the effect of buffering can be seen. Type
 * CONTROL-d to terminate.
 *
 * Note the mutual exclusion on access to the head and tail pointers of the
 * buffer and on clearing and setting the full and empty semaphores.  For a
 * discussion, see Peterson & Silberschatz, "Operating System Concepts",
 * 2nd ed., chapter 9.
 *
 * Be aware when running this that the system does keyboard buffering
 * underneath this program, so you can type ahead more than you might think.
 *
 * Needs shrchild.c as companion program.
 *
 * Compile as: cl -AL -G2 -Lp share.c
 *
 * Created by Microsoft Corp. 1986
 */

#define INCL_DOSPROCESS
#define INCL_SUB
#define INCL_DOSMEMMGR

#include <os2def.h>
#include <bse.h>
#include "share.h"		/* common declarations between processes */

#define	CHILD1PROG	"SHRCHILD.EXE"		/* child process */
#define	CTRLd		4			/* termination character */
#define FBSZ		32

KBDKEYINFO KeyData;	    /* declared in subcalls.h */

main()
{
	SEL		Selector;
	RESULTCODES	childID;     /* child process id */
	unsigned	rc;		/* return code */
	char		c,
			fbuf[FBSZ];	/* failing object buffer */
	struct ShareRec *SmemPtr;	/* shared memory pointer */

	/* allocate the shared memory segment */
	if (rc = DosAllocShrSeg( SHRSEGSIZE, (PSZ)SHRSEGNAME,
				&Selector ))  {
		printf("alloc of shared memory failed, error: %d\n", rc);
		DosExit(EXIT_PROCESS, 0);
	}

	/* Get a far pointer from a 16 bit selector */
	SmemPtr = (struct ShareRec *) MAKEP(Selector, 0);

	/* Initialize circular buffer flags */
	DosSemClear((HSEM)&(SmemPtr->fullsem));
	DosSemSet((HSEM)&(SmemPtr->emptysem));
	DosSemClear((HSEM)&(SmemPtr->mutexsem));
	SmemPtr->head = 0;
	SmemPtr->tail = 0;

	/* exec asynchronously the consumer process */
	if (rc = DosExecPgm(
		(PCHAR) fbuf,			/* ObjNameBuf		*/
		FBSZ,				/* ObjNameLen		*/
		EXEC_ASYNC,			/* AsyncTraceFlags	*/
		(PSZ) 0L,			/* Argument Strings	*/
		(PSZ) 0L,			/* Environment Strings	*/
		&childID,
						/* ID & Termination Codes */
		(PSZ) CHILD1PROG )) {		/* Program Filename	*/
	    
		printf("exec of child process failed, error: %d\n", rc);
		DosExit(EXIT_PROCESS, 0);
	}

	/* Here, we read chars from the keyboard, and put them in a */
	/* circular buffer in shared memory */

	KbdCharIn(&KeyData, 0, 0);	/* read character from keyboard */

	while((c = KeyData.chChar) != CTRLd) {

	    /* block if buffer full */
	    DosSemWait((HSEM)&(SmemPtr->fullsem), WAITFOREVER);

	    /* mutual exclusion on buffer pointers and semaphores */
	    DosSemRequest((HSEM)&(SmemPtr->mutexsem), WAITFOREVER);
	    SmemPtr->CircBuffer[SmemPtr->head] = c;
	    SmemPtr->head++;		    /* step pointer */
	    SmemPtr->head %= CIRCBUFSIZE;   /* wrap at end  */
	    if(BUFFUL(SmemPtr))
		DosSemSet((HSEM)&(SmemPtr->fullsem)); /* indicate buf full   */
	    DosSemClear((HSEM)&(SmemPtr->emptysem));  /* indicate buf !emtpy */
	    DosSemClear((HSEM)&(SmemPtr->mutexsem));


	    KbdCharIn(&KeyData, 0, 0);	/* read character from keyboard */
	}
	DosKillProcess( 1, childID.codeTerminate);	/* kill consumer */
	DosExit( EXIT_PROCESS, 0 );	   /* exit, terminating all threads */
}
