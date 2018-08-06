/* qserver.c RHS 5/1/89
 *
 *
 This program demonstrates the use of queues and creates a server process
 that client processes can pass messages to. To use it, start one instance
 of this program with:

 QSERVER

 Then start multiple instances of QCLIENT.C with:

 QCLIENT

 *
 */

#define INCL_DOS
#define INCL_DOSQUEUES
#define INCL_KBD
#define INCL_ERRORS

#include<os2.h>
#include<stdio.h>

#include"q.h"

void error_exit(USHORT err, char *msg);
void main(void);

void main(void)
	{
	USHORT retval, qcount = 0;
	KBDKEYINFO kbdkeyinfo;
	PCH p;
	HQUEUE qhandle;
	QUEUERESULT qresult;
	USHORT ellength;
	BYTE priority;

	if(retval = DosCreateQueue(&qhandle,QUE_PRIORITY,QUEUENAME))
		error_exit(retval,"DosCreateQueue");

	printf("Server has opened queue, awaiting messages...\n");

	while(TRUE)
		{
		if(retval = DosReadQueue(qhandle,&qresult,&ellength,
				(PVOID FAR *)&p,0x0000,DCWW_WAIT,&priority,0L))
			{
			if(retval != ERROR_QUE_EMPTY)	/* if error was not from empty queue */
				error_exit(retval,"DosReadQueue");
			continue;
			}
		DosQueryQueue(qhandle,&qcount);
		printf(
			"Server: (%u pending) message received from %u, priority %02u: %s\n",
				qcount,qresult.pidProcess,priority,p);

		DosFreeSeg(SELECTOROF(p));

		KbdCharIn(&kbdkeyinfo,IO_NOWAIT,0);			/* check for key press		*/
		if(kbdkeyinfo.fbStatus & FINAL_CHAR_IN)	/* if pressed, break out	*/
			break;
		}
	DosCloseQueue(qhandle);
	DosExit(EXIT_PROCESS,0);							/* and exit						*/
	}

void error_exit(USHORT err, char *msg)
	{
	printf("Error %u returned from %s\n",err,msg);
	DosExit(EXIT_PROCESS,0);
	}






















