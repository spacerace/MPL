/* shmem.c RHS 5/2/89
 *
 This program demonstrates the use of shared memory. To use it, run more than
 one copy of the program in different sessions with:

 SHMEM

 */

#define INCL_DOS
#define INCL_KBD
#define INCL_ERRORS

#include<os2.h>
#include<stdio.h>
#include<memory.h>

#define	SHAREDMEM		"\\sharemem\\shmem.mem"
#define	SEMNAME			"\\sem\\ssem.sem"
#define	SHAREDMEMSIZE	200

void error_exit(USHORT err, char *msg);
PID Os2GetPid(void);
void main(void);

void main(void)
	{
	HSYSSEM semhandle;
	SEL selector;
	USHORT retval;
	int creator = FALSE;
	KBDKEYINFO kbdkeyinfo;
	PCH p;
	PID pid;

	pid = Os2GetPid();

		/* first process will successfully execute this code and create the
			the semaphore
		 */
	if(retval = DosCreateSem(CSEM_PUBLIC,&semhandle,SEMNAME))
		{
		if(retval != ERROR_ALREADY_EXISTS)
			error_exit(retval,"DosCreateSem");

		/* second process will open the semaphore previously created	*/
		else if (retval = DosOpenSem(&semhandle,SEMNAME))
			error_exit(retval,"DosOpenSem");

		if(retval = DosGetShrSeg(SHAREDMEM,&selector))
			error_exit(retval,"DosGetShrSeg");

		DosSemWait(semhandle,SEM_INDEFINITE_WAIT);
		DosSemRequest(semhandle,SEM_INDEFINITE_WAIT);
		}
	else
		{
		/* then first process will wait here until second process executes
			the code in the loop below
		 */
		if(retval = DosAllocShrSeg(SHAREDMEMSIZE,SHAREDMEM,&selector))
			error_exit(retval,"DosAllocShrSeg");

		DosSemSet(semhandle);
		creator = TRUE;
		}

	p = MAKEP(selector,0);								/* create pointer to shmem	*/
	if(!creator)
		memset(p,'\0',SHAREDMEMSIZE);

	while(TRUE)	/* both processes will continue to execute in this loop		*/
		{
		if(*p)
			printf("%s (%u) received: %s\n",(creator ? "Owner" : "User"),pid,p);

		sprintf(p,(creator ?
			   "from %u, the owner of this shared memory...Hello!" :
				"from %u,    a user of this shared memory...Hello!"),pid);

		DosSemClear(semhandle);

		KbdCharIn(&kbdkeyinfo,IO_NOWAIT,0);			/* check for key press		*/
		if(kbdkeyinfo.fbStatus & FINAL_CHAR_IN)	/* if pressed, break out	*/
			break;

		DosSleep(150L);
		DosSemRequest(semhandle,SEM_INDEFINITE_WAIT);
		}

	DosCloseSem(semhandle);								/* close the semaphore		*/
	DosFreeSeg(selector);
	DosExit(EXIT_PROCESS,0);							/* and exit						*/
	}


PID Os2GetPid(void)										/* returns process id		*/
	{
	PIDINFO pidinfo;

	DosGetPID(&pidinfo);									/* get process id				*/
	return pidinfo.pid;									/* return it					*/
	}

void error_exit(USHORT err, char *msg)
	{
	printf("Error %u returned from %s\n",err,msg);
	DosExit(EXIT_PROCESS,0);
	}





