/* sig.c RHS 5/1/89
 *
 * signal handler example
 *
	This program lets you observe how one process can use the OS2 signalling
 	mechanism to pass signals between two processes. To use it, first run the
	program in one session with:

	SIG

	Note the process ID it prints. This will be the receiver of the signals.
	Then you can run subsequent instances of SIG in other sessions by passing
	them the process ID of the first instance as a command line argument:

	SIG n

	where 'n' is the process ID printed by the first instance.

 */

#define INCL_DOS
#define INCL_VIO
#define INCL_KBD
#define INCL_ERRORS

#include<os2.h>
#include<mt\stdio.h>
#include<mt\memory.h>
#include<mt\string.h>
#include<mt\stdlib.h>
#include<mt\process.h>

#define	SEMNAME	"\\sem\\ssem.sem"

#define	THREADSTACK	400
char keyboard_thread_stack[THREADSTACK];

unsigned pid = 0;
unsigned signaler = FALSE;
USHORT prevaction0, prevaction1, prevaction2, prevaction3, prevaction4,
	prevaction5;
PFNSIGHANDLER prevhandler0, prevhandler1, prevhandler2, prevhandler3,
	prevhandler4, prevhandler5;

#define MAXSIGS		3

char *signames[MAXSIGS] =	{	"Flag A", "Flag B",	"Flag C"	};

USHORT signum[MAXSIGS] =	{	PFLG_A, PFLG_B, PFLG_C };

void main(int argc, char **argv);
PID OS2GetPid(void);
void APIENTRY sighandler(USHORT arg, USHORT num);
void error_exit(USHORT err, char *msg);
void keyboard_thread(void);


void APIENTRY sighandler(USHORT arg, USHORT num)
	{
	USHORT retval;
	PFNSIGHANDLER prevhandler = NULL;
	USHORT prevaction = 0;

		/* acknowledge immediately		*/
	retval = DosSetSigHandler((PFNSIGHANDLER)sighandler,&prevhandler,
			&prevaction,SIGA_ACKNOWLEDGE,num);
	
	switch(num)
		{
		case SIG_CTRLC:
			printf("%u received ^C...",pid);
			break;
		case SIG_KILLPROCESS:
			printf("%u notified of pending termination, terminating...",pid);
			DosExit(EXIT_PROCESS,0);
			break;
		case SIG_CTRLBREAK:
			printf("%u received ^Break...",pid);
			break;
		case SIG_PFLG_A:						/* FLAG_A received			*/
			printf("%u received signal, Flag A, arg=%u...",pid,arg);
			break;
		case SIG_PFLG_B:
			printf("%u received signal, Flag B, arg=%u...",pid,arg);
			break;
		case SIG_PFLG_C:
			printf("%u received signal, Flag C, arg=%u...",pid,arg);
			break;
		default:
			printf("%u received unknown signal (%u), arg=%u", pid, num, arg);
			break;
		}

	if(retval)
		printf("unable to acknowledge signal, retval=%u.\n",retval);
	else
		printf("acknowledged.\n");
	return;
	}

void main(int argc, char **argv)
	{
	unsigned retval,receiver_pid, signal = 0;
	HSYSSEM semhandle;
	KBDKEYINFO kbdkeyinfo;

	pid = OS2GetPid();
	printf("%s loaded, pid=%u\n",argv[0],pid);

	if(retval = DosCreateSem(CSEM_PUBLIC,&semhandle,SEMNAME))
		{
		if(retval != ERROR_ALREADY_EXISTS)
			error_exit(retval,"DosCreateSem");

		signaler = TRUE;										/* set flag 				*/
		if(argc != 2)
			error_exit(0,"main");
		receiver_pid = atoi(argv[1]);
		}
	else
		{
		if(_beginthread(keyboard_thread,keyboard_thread_stack,THREADSTACK,NULL)
				== -1)
			error_exit(-1,"_beginthread");

			/* set up signal handler to be sighandler function, pass address
				of prevhandler and prevaction even though there weren't any
				previous.
			 */
		if(retval = DosSetSigHandler(sighandler,&prevhandler0,&prevaction0,
				SIGA_ACCEPT,SIG_CTRLC))
			error_exit(retval,"DosSetSigHandler(0)");
		if(retval = DosSetSigHandler(sighandler,&prevhandler1,&prevaction1,
				SIGA_ACCEPT,SIG_KILLPROCESS))
			error_exit(retval,"DosSetSigHandler(1)");
		if(retval = DosSetSigHandler(sighandler,&prevhandler2,&prevaction2,
				SIGA_ACCEPT,SIG_CTRLBREAK))
			error_exit(retval,"DosSetSigHandler(2)");
		if(retval = DosSetSigHandler(sighandler,&prevhandler3,&prevaction3,
				SIGA_ACCEPT,SIG_PFLG_A))
			error_exit(retval,"DosSetSigHandler(3)");
		if(retval = DosSetSigHandler(sighandler,&prevhandler4,&prevaction4,
				SIGA_ACCEPT,SIG_PFLG_B))
			error_exit(retval,"DosSetSigHandler(4)");
		if(retval = DosSetSigHandler(sighandler,&prevhandler5,&prevaction5,
				SIGA_ACCEPT,SIG_PFLG_C))
			error_exit(retval,"DosSetSigHandler(5)");
		}

	while(TRUE)
		{
		if(signaler)										/* if another copy out there*/
			{
				/* signal it (and not its children) with FLAG_A */
			if(retval = DosFlagProcess(receiver_pid,FLGP_PID,signum[signal],
					pid))
				error_exit(retval,"DosFlagProcess");
			else
				printf("Sender (%u): sent signal to %u: %s\n",pid,
					receiver_pid, signames[signal]);
			if(++signal >= MAXSIGS)
				signal = 0;
																/* check for key press		*/
			retval = KbdCharIn(&kbdkeyinfo,IO_NOWAIT,0);
			if(!retval || (retval != ERROR_SIGNAL_PENDING))
																/* if pressed, break out	*/
				if(kbdkeyinfo.fbStatus & FINAL_CHAR_IN)
					break;
			DosSleep(100L);								/* sleep again					*/
			}
		else
			DosSleep(10000L);								/* don't wake too often		*/
		}
	}


PID OS2GetPid(void)
	{
	PLINFOSEG ldt;
	SEL gdt_descriptor, ldt_descriptor;   /* infoseg descriptors */

	if(DosGetInfoSeg(&gdt_descriptor, &ldt_descriptor))
		return 0;

	ldt = MAKEPLINFOSEG(ldt_descriptor);
	return ldt->pidCurrent;
	}


void error_exit(USHORT err, char *msg)
	{
	printf("Error %u returned from %s\n",err,msg);
	DosExit(EXIT_PROCESS,0);
	}

void keyboard_thread(void)
	{
	KBDKEYINFO kbdkeyinfo;

	while(TRUE)
		{
		if(!KbdCharIn(&kbdkeyinfo,IO_WAIT,0))
			DosExit(EXIT_PROCESS,0);
		}
	}




