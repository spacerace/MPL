/* qclient.c RHS 5/1/89
 *
 *
 This program demonstrates the use of queues. To use it, you should first
 start QSERVER.C with:

 QSERVER

 Then you can run multiple copies of this program with:

 QCLIENT

 */
#define INCL_DOS
#define INCL_KBD
#define INCL_ERRORS

#include<os2.h>
#include<string.h>
#include<stdio.h>
#include"q.h"

char *p = "Hello!";
#define MAX_PRIORITY	15

void error_exit(USHORT err, char *msg);
PID Os2GetPid(void);
void main(void);

void main(void)
	{
	USHORT retval;
	KBDKEYINFO kbdkeyinfo;
	HQUEUE qhandle;
	BYTE priority = 0;
	SEL selector,qownersel;
	PID qowner;
	PCH ptr;
	int len;
	PID pid;

	pid = Os2GetPid();

	while(TRUE)
		{
		if(!(retval = DosOpenQueue(&qowner,&qhandle,QUEUENAME)))
			break;
		if(retval != ERROR_QUE_NAME_NOT_EXIST)
			error_exit(retval,"DosOpenQueue");
		else
			{
			DosSleep(1000L);
			continue;
			}
		}
	printf("Client (%u) has opened queue, now messaging...\n",pid);
	
	while(TRUE)
		{
		if(retval = DosAllocSeg(len = (strlen(p)+1),&selector,SEG_GIVEABLE))
			error_exit(retval,"DosAllocSeg");

		ptr = MAKEP(selector,0);
		strcpy(ptr,p);
		if(retval = DosGiveSeg(selector,qowner,&qownersel))
			error_exit(retval,"DosGiveSeg");

		ptr = MAKEP(qownersel,0);

		if(retval = DosWriteQueue(qhandle,0,len,(PBYTE)ptr,priority))
			error_exit(retval,"DosWriteQueue");

		DosFreeSeg(selector);
		printf("Client (%u) sent message, priority %u\n",pid,priority);

		KbdCharIn(&kbdkeyinfo,IO_NOWAIT,0);			/* check for key press		*/
		if(kbdkeyinfo.fbStatus & FINAL_CHAR_IN)	/* if pressed, break out	*/
			break;
		if(++priority > MAX_PRIORITY)
			priority = 0;
		DosSleep(50L);
		}

	DosCloseQueue(qhandle);
	DosExit(EXIT_PROCESS,0);							/* and exit						*/
	}

void error_exit(USHORT err, char *msg)
	{
	printf("Error %u returned from %s\n",err,msg);
	DosExit(EXIT_PROCESS,0);
	}

PID Os2GetPid(void)										/* returns process id		*/
	{
	PIDINFO pidinfo;

	DosGetPID(&pidinfo);									/* get process id				*/
	return pidinfo.pid;									/* return it					*/
	}


