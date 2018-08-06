/* npclient.c RHS 5/1/89
 *
 This program creates a client process which will communicate with an
 instance of the named pipe created by NPSERVER. After running NPSERVER,
 you can run several instances of this program with:

 NPCLIENT


 */
#define INCL_DOS
#define INCL_KBD
#define INCL_ERRORS

#include<os2.h>
#include<string.h>
#include<stdio.h>
#include"nmpipe.h"

char writebuf[PIPESIZE];

void main(void);
void error_exit(USHORT err, char *msg);
PID Os2GetPid(void);


void main(void)
	{
	HPIPE pipehandle;
	USHORT retval;
	USHORT bytesread;
	USHORT action;
	PID pid;

	pid = Os2GetPid();
	printf("Client %u trying to open named pipe...\n",pid);
	while(TRUE)												/* open the pipe				*/
		{
		if(retval = DosOpen(NAMEDPIPE,&pipehandle,&action,0L,FILE_NORMAL,
				FILE_OPEN,OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,0L))
			{
			printf("%u: Error %u trying to open %s, sleeping...\n",pid,retval,
					NAMEDPIPE);
			if(retval == ERROR_PIPE_BUSY)
				while(TRUE)
					{
					if(!(retval = DosWaitNmPipe(NAMEDPIPE,1000L)))
						break;
					if(retval != ERROR_SEM_TIMEOUT)
						error_exit(retval,"DosWaitNmPipe");
					}
			printf("Trying again...\n");
			}
		else
			break;
		}
	
	printf("Client %u: server has connected pipe, sending message...\n",pid);

	while(TRUE)
		{
		strcpy(writebuf,"Anything I want?");		/* write a message			*/
		if(retval = DosWrite(pipehandle,writebuf,strlen(writebuf)+1,&bytesread))
			error_exit(retval,"DosWrite");

																/* read acknowledgment		*/
		if(retval = DosRead(pipehandle,writebuf,PIPESIZE,&bytesread))
			error_exit(retval,"DosRead");
		printf("%u: Server has acknowledged: \'%s\'\n",pid,writebuf);
		DosSleep(200L);
		}
	DosClose(pipehandle);
	DosExit(EXIT_PROCESS,0);
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
