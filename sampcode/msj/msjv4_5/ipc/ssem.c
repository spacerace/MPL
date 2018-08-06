/* ssem.c RHS 5/1/89
 *
 This program demonstrates the use of system semaphores. To use it, run
 more than one copy of the program in different sessions with:

 SSEM

 *
 */

#define INCL_DOS
#define INCL_KBD
#define INCL_ERRORS

#include<os2.h>
#include<stdio.h>


#define	SEMNAME	"\\sem\\ssem.sem"

void error_exit(USHORT err, char *msg);
PID Os2GetPid(void);
void main(void);

void main(void)
	{
	HSYSSEM semhandle;
	USHORT retval;
	int creator = FALSE;
	KBDKEYINFO kbdkeyinfo;
	PID pid;
	unsigned count = 0;

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

		DosSemWait(semhandle,SEM_INDEFINITE_WAIT);
		DosSemRequest(semhandle,SEM_INDEFINITE_WAIT);
		}
	else
		{
		/* then first process will wait here until second process executes
			the code in the loop below
		 */
		DosSemSet(semhandle);
		creator = TRUE;
		}

	while(TRUE)	/* both processes will continue to execute in this loop	*/
		{
		printf("%s (%u) %u\n",(creator ? "Creating Thread" : "User Thread"),
				pid,count++);

		DosSemClear(semhandle);
		
		KbdCharIn(&kbdkeyinfo,IO_NOWAIT,0);			/* check for key press		*/
		if(kbdkeyinfo.fbStatus & FINAL_CHAR_IN)	/* if pressed, break out	*/
			break;

		DosSleep(150L);
		DosSemRequest(semhandle,SEM_INDEFINITE_WAIT);
		}

	DosCloseSem(semhandle);								/* close the semaphore		*/
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




