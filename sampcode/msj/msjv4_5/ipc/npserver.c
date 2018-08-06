/* npserver.c RHS 5/1/89
 *
 This program demonstrates the use of named pipes by creating a server
 process which will create more than instance of a named pipe. To see how
 this works, run this program with:

 NPSERVER

 Then run multiple instances of the NPCLIENT program in other sessions.

 You can also run the NPRMODE program in the compatibility box to talk to
 the server.

 */
#define INCL_DOS

#include<os2.h>
#include<mt\string.h>
#include<mt\stdio.h>
#include<mt\process.h>
#include"nmpipe.h"

#define	MAXSERVERS			5
#define	THREADSTACKSIZE	1200

unsigned long semhandle = 0L;
typedef struct _servers
	{
	char	stack[THREADSTACKSIZE];
	int	threadID;
	} SERVER;

SERVER servers[MAXSERVERS];
USHORT pipeopenmode = PIPE_ACCESS_DUPLEX;
USHORT pipemode = (PIPE_WAIT | PIPE_READMODE_BYTE | PIPE_TYPE_BYTE |	MAXSERVERS);

void main(void);
void error_exit(USHORT err, char *msg);
void server_thread(int servernum);

void main(void)
	{
	int i;

	DosSemSet(&semhandle);

	for( i = 0; i < MAXSERVERS; i++)					/* create server threads	*/
		{
		if(servers[i].threadID = _beginthread(server_thread,servers[i].stack,
				THREADSTACKSIZE,(void *)i)	== -1)
			error_exit(-1,"_beginthread");
		DosSleep(100L);
		}

	DosSemRequest(&semhandle,SEM_INDEFINITE_WAIT);/* wait for semaphore		*/
	DosExit(EXIT_PROCESS,0); 	   					/* kill the process			*/
	}	

void server_thread(int servernum)
	{
	char readbuf[PIPESIZE];
	HPIPE pipehandle;
	USHORT retval;
	USHORT bytesread;

	printf("Server instance %d creating Named pipe...\n",servernum);
																/* create pipe instance		*/
	if(retval = DosMakeNmPipe(NAMEDPIPE,&pipehandle,pipeopenmode,pipemode,
			PIPESIZE,PIPESIZE,1000L))
		error_exit(retval, "DosMakeNmPipe");

	printf("Server instance %d waiting for Client to open pipe...\n",servernum);

	if(retval = DosConnectNmPipe(pipehandle))		/* wait until connected		*/
		error_exit(retval,"DosConnectNmPipe");

	printf("Server instance %d: Client has opened pipe, waiting for message...\n",servernum);

	while(TRUE)
		{
																/* read a message				*/
		if(retval = DosRead(pipehandle,readbuf,PIPESIZE,&bytesread))
			error_exit(retval,"DosRead");

		printf("Server instance %d received: \'%s\'...acknowledging\n",
				servernum,readbuf);
																/* acknowledge it				*/
		if(retval = DosWrite(pipehandle,ACKNOWLEDGE,strlen(ACKNOWLEDGE)+1,
				&bytesread))
			error_exit(retval,"DosWrite");
		DosSleep(250L);
		}
	DosSemClear(&semhandle);
	DosClose(pipehandle);
	}

void error_exit(USHORT err, char *msg)
	{
	printf("Error %u returned from %s\n",err,msg);
	DosExit(EXIT_PROCESS,0);
	}
