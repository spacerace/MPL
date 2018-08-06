/*
 * This is the child process exec'd asynchronously from share.c.
 *
 * Created by Microsoft Corp. 1986
 */

#define INCL_DOSMEMMGR
#define INCL_DOSSEMAPHORES
#define INCL_DOSFILEMGR
#define INCL_DOSPROCESS

#include <os2def.h>
#include <bsedos.h>		/* necessary whenever doscalls are made */
#include "share.h"		/* common shared memory declarations */

main()
{
	SEL			Selector;
	unsigned		rc;		/* return code */
	USHORT			nBytes;
	char			c;
	struct ShareRec 	*SmemPtr;	/* shared memory pointer */

	/* get selector to the shared memory segment defined in parent */

	if (rc = DosGetShrSeg( (PSZ) SHRSEGNAME,
				&Selector ))  {
		DosExit(EXIT_PROCESS, 0);
	}

	/* Get a far pointer from a 16 bit selector */

	SmemPtr = (struct ShareRec *) MAKEP(Selector, 0);
	
	for (;;) {

	    /* block if buffer empty */
	    DosSemWait((HSEM)&(SmemPtr->emptysem), WAITFOREVER);

	    /* mutual exclusion on buffer pointers and semaphores */
	    DosSemRequest((HSEM)&(SmemPtr->mutexsem), WAITFOREVER);
	    c = SmemPtr->CircBuffer[SmemPtr->tail];	/* get next char */
	    SmemPtr->tail++;		    /* step pointer */
	    SmemPtr->tail %= CIRCBUFSIZE;   /* wrap at end  */
	    if (SmemPtr->tail == SmemPtr->head)
		DosSemSet((HSEM)&(SmemPtr->emptysem)); /* indicate buf empty */
	    DosSemClear((HSEM)&(SmemPtr->fullsem));    /* indicate buf !full */
	    DosSemClear((HSEM)&(SmemPtr->mutexsem));

	    DosWrite( 1, (PVOID) &c, 1, &nBytes );     /* display next char */
	    DosSleep(1000L);	/* max 1 per sec to demo buffering & sems */
	}
}
