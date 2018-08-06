/* pipe.c RHS 5/2/89
 *
 * pipe example:
 *
 * this program will reroute the stdout of a child process to the pipe,
 * then it will print the child's output to the upper portion of the screen
 * while you can type into the lower portion of the screen
 *
 * To run:
 *
 *		PIPE <child program name> [child program arguments]
 *
 */

#define INCL_DOS
#define INCL_KBD
#define INCL_VIO
#define INCL_ERRORS

#include<os2.h>
#include<mt\stdio.h>
#include<mt\process.h>
#include<mt\string.h>

#define 	PIPESIZE 	80
#define	STDIN			0
#define	STDOUT		1
#define	FAILBUFLEN	128
#define	ENV			0L
#define	THREADSTACK	800

char readbuf[PIPESIZE+5];
HFILE readhandle, writehandle, newSTDOUT = STDOUT;
USHORT cell = 0x1f20;
char readchild_stack[THREADSTACK];


void error_exit(USHORT err, char *msg);
PID Os2GetPid(void);
void main(int argc, char **argv);
void readchild(void);

void main(int argc, char **argv)
	{
	USHORT retval, cell2 = 0x1fc4, i;
	STRINGINBUF stringinbuf;
	char failbuf[FAILBUFLEN];
	RESULTCODES resultcodes;
	char keybuf[PIPESIZE], *p;
	PID pid,pidasync;

	if(argc < 2)											/* must have one argument	*/
		error_exit(0,"main");

																/* create pipe					*/
	if(retval = DosMakePipe( &readhandle, &writehandle, PIPESIZE))
		error_exit(retval,"DosMakePipe");

	DosClose(STDOUT);										/* close stdout				*/

														/* pipe write handle now stdout	*/
	if(retval = DosDupHandle(writehandle,&newSTDOUT))
		error_exit(retval,"DosDupHandle");

																/* clear the screen			*/
	VioScrollUp(0,0,0xffff,0xffff,0xffff,(PBYTE)&cell,0);
	VioWrtNCell((PBYTE)&cell2,80,11,0,0);			/* draw line to split screen*/

	memset(readbuf,0,sizeof(readbuf));				/* clear buffer				*/
	strcpy(readbuf,argv[1]);							/* add program name			*/
	if(!(strstr(strupr(readbuf),".EXE")))			/* tack on .EXE				*/
		strcat(readbuf,".EXE");

	p = (&readbuf[strlen(readbuf)]+1);
	for( i = 2; i < argc; i++)							/* add program arguments	*/
		{
		strcat(p,argv[i]);
		strcat(p," ");
		}
																/* run the child program	*/
	if(retval = DosExecPgm(failbuf,FAILBUFLEN,EXEC_ASYNC,readbuf,ENV,
			&resultcodes, readbuf))
		error_exit(retval,"DosExecPgm");
	pidasync = resultcodes.codeTerminate;

																/* start pipe read thread	*/
	if(_beginthread(readchild,readchild_stack,THREADSTACK,NULL) == -1)
		error_exit(-1,"_beginthread");

	while(TRUE)
		{
		stringinbuf.cb = sizeof(keybuf)-1;

																/* await operator input		*/
		if(retval = KbdStringIn(keybuf,&stringinbuf,IO_WAIT,0))
			error_exit(retval,"KbdStringIn");

		keybuf[stringinbuf.cchIn] = '\0';
																/* scroll screen up			*/
		VioScrollUp(12,0,23,0xffff,1,(PBYTE)&cell,0);
																/* write command				*/
		VioWrtCharStr(keybuf,strlen(keybuf),23,0,0);
																/* clear prompt line			*/
		VioWrtNCell((PBYTE)&cell,80,24,0,0);
		if(!strcmp(strupr(keybuf),"QUIT"))			/* if QUIT command, get out*/
			break;
		}
																/* see if child finished	*/
	if(retval = DosCwait(DCWA_PROCESSTREE,DCWW_NOWAIT,&resultcodes,&pid,
			resultcodes.codeTerminate))
		{
		if(retval == ERROR_CHILD_NOT_COMPLETE)		/* if not, kill it			*/
			{
			if(retval = DosKillProcess(DKP_PROCESSTREE,pidasync))
				error_exit(retval,"DosKillProcess");
			}
		else if(retval != ERROR_WAIT_NO_CHILDREN)
			error_exit(retval,"DosCwait");
		}

	DosExit(EXIT_PROCESS,0);							/* get out						*/
	}


void readchild(void)		/* this thread reads the pipe, and prints the lines*/
	{
	USHORT retval,bytesread,i, lines;
	char *p = readbuf;

	while(TRUE)
		{														/* read the pipe				*/
		if(retval = DosRead(readhandle,p,PIPESIZE-(p-readbuf),&bytesread))
			error_exit(retval,"DosRead");
		bytesread += (p-readbuf);
						
		for(i = lines =0; i < bytesread; i++)		/* remove CRs and LFs		*/
			{
			if(readbuf[i] == '\n')
				lines++;
			if(readbuf[i] == '\r' || readbuf[i] == '\n')
				readbuf[i] = '\0';
			}
																/* print each line found	*/
		for(p = readbuf, i = 0; i < bytesread && lines; lines--)
			{
			VioScrollUp(0,0,10,0xffff,1,(PBYTE)&cell,0);
			VioWrtCharStr(p,strlen(p),10,0,0);
			i += strlen(p);
			p += strlen(p);
			for( ; !(*p) && i < bytesread; i++, p++);
			DosSleep(1L);
			}

		if(*p && !lines && i < bytesread)			/* if anything leftover		*/
			{
			memmove(readbuf,p,bytesread-i);			/* move it up					*/
			p = &readbuf[bytesread-i];
			}
		else
			p = readbuf;
		}
	}

PID Os2GetPid(void)										/* returns process id		*/
	{
	PIDINFO pidinfo;

	DosGetPID(&pidinfo);									/* get process id				*/
	return pidinfo.pid;									/* return it					*/
	}

void error_exit(USHORT err, char *msg)
	{
	char buf[100];

	sprintf(buf,"Error %u returned from %s\r\n",err,msg);
	VioWrtTTY(buf,strlen(buf),0);
	DosExit(EXIT_PROCESS,0);
	}
