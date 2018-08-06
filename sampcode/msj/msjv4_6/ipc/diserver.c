/* diserver.c RHS 5/28/89
 *
 * directory information server
 
	Run this program before running any programs (like DIPOP or LS) that
	use the server. Run the program with:

	DISERVER [/v]

	The optional /v will turn on verbose mode, and status messages are printed.
 */

#define	INCL_DOS
#define	INCL_ERRORS

#include<os2.h>
#include<mt\stdio.h>
#include<mt\process.h>
#include<mt\string.h>
#include"errexit.h"

#define DICODE
#include"di.h"

#define	THREADSTACK	1000
#define	MAXTHREADS	20
#define	lastchar(str)		(str[strlen(str)-1])
typedef struct _serverthreads
	{
	ULONG					threadSem;
	SHORT					threadID;
	PREQUESTHEADER		request;
	SHORT					threadnum;
	UCHAR					threadstack[THREADSTACK];
	} SERVERTHREAD;

SERVERTHREAD SERVERTHREADS[MAXTHREADS];

USHORT verbose = FALSE;

void server_thread(SERVERTHREAD *threaddata);

void main(int argc, char **argv);

void main(int argc, char **argv)
	{
	USHORT			retval, qcount = 0, nextthread = 0;
	HQUEUE 			qhandle;
	QUEUERESULT 	qresult;
	USHORT 			ellength;
	BYTE				priority;
	PREQUESTHEADER	request;

	if(argc > 1)
		if(((*argv[1]=='-') || (*argv[1]=='/')) &&
				(argv[1][1]=='v' || argv[1][1]=='V'))
			verbose = TRUE;
		else
			printf("DiServer: ignoring unknown argument \"%s\"\n",argv[1]);

	/* create the server queue for taking requests from clients
	 */

	if(retval = DosCreateQueue(&qhandle,QUE_FIFO,DIRINFOQNAME))
		error_exit(retval,"DosCreateQueue");

	if(verbose)
		printf("DiServer: queue is open, awaiting requests...\n");

	memset(SERVERTHREADS,0,sizeof(SERVERTHREADS));

	while(TRUE)
		{
			/* read the queue to see if any requests have been made	*/
		if(retval = DosReadQueue(qhandle,&qresult,&ellength,
				(PVOID FAR *)&request,0x0000,DCWW_WAIT,&priority,0L))
			if(retval != ERROR_QUE_EMPTY)	/* if error was not from empty queue */
				error_exit(retval,"DosReadQueue");

		DosQueryQueue(qhandle,&qcount);
		if(verbose)
			printf("DiServer: request received from process %u (%u pending)\n",
						qresult.pidProcess,qcount);

		/* request has been made:
			o	find the next available server thread structure
			o	set its semaphore so it will wait until you've started it
			o	start the new server thread
			o	once it's successfully started, set the request pointer in
				its data structure to the request that was received
			o	then tell the thread to go ahead by clearing the semaphore--
				thus the thread can't go ahead without the request and without
				having it's own thread ID installed in it's structure
			o	if the next thread structure isn't available, find one that is
			o	if it goes all the way around and doesn't find one, it will sleep
				for a second before it tries again
		 */

		while(TRUE)
			{
			if(nextthread == MAXTHREADS)
				{
				nextthread = 0;
				DosSleep(1000L);
				}

			if(!SERVERTHREADS[nextthread].threadID)
				{
				DosSemSet(&SERVERTHREADS[nextthread].threadSem);
				if((SERVERTHREADS[nextthread].threadID = _beginthread(
						server_thread,	SERVERTHREADS[nextthread].threadstack,
						THREADSTACK,(void *)&SERVERTHREADS[nextthread]))
						== -1)
					error_exit(-1,"_beginthread");
				else
					{
					SERVERTHREADS[nextthread].request = request;
					SERVERTHREADS[nextthread].threadnum = nextthread;
					DosSemClear(&SERVERTHREADS[nextthread].threadSem);
					nextthread++;
					break;
					}
				}
			while(++nextthread < MAXTHREADS)
				if(!SERVERTHREADS[nextthread].threadID)
					break;
			}

		}
	DosCloseQueue(qhandle);
	DosExit(EXIT_PROCESS,0);							/* and exit						*/
	}

void server_thread(SERVERTHREAD *threaddata)
	{
	PREQUESTHEADER request;
	USHORT i, retval,found, bufsize, attributes,count;
	PDIRINFORESULT	results;
	PFILEFINDBUF findbuf;
	HDIR dirhandle;
	char filespec[80],dircheck = FALSE;
	char far *temp;

		/* wait until main thread says to continue	*/
	DosSemRequest(&threaddata->threadSem, SEM_INDEFINITE_WAIT);
	request = threaddata->request;
	if(verbose)
		{
		printf("Thread #%u:\n",threaddata->threadnum);
		printf("\t%u filespec(s) in request\n",request->numRequests);
		printf("\tClient's current directory is %s\n",request->currentdir);
		}

	findbuf = request->resultptr;
	temp = MAKEP(request->serverwsel,0);

		/* process each request:
			o	build the complete path and filespec from currentdir, and filespec
			o	get the attributes
			o	if there are attributes, include the directory bit so the dirs
				will be found
				else set the directory bit
			o	set bufsize to the max segment size minus the space already used
				by results already found
		 */

	for(request->totalresults = i = 0, results = &request->resultArray[0];
			i < request->numRequests; i++, results++)
		{
		if(verbose)
			printf("\tFilespec %u: Attr:%4x Dirspec: %s Filespec: %s\n",
					i,results->attributes,results->currentdir,results->filespec);
		strcpy(filespec,results->currentdir);
		if(lastchar(filespec) != '\\')
			strcat(filespec,"\\");
		strcat(filespec,results->filespec);
		attributes = results->attributes;
		if(attributes)
			attributes |= FILE_DIRECTORY;
		else
			attributes = FILE_DIRECTORY;

expand_directory:
		found = 0xffff;
		dirhandle = HDIR_SYSTEM;
		bufsize = MAXSEGSIZE-1;
		bufsize -= ((char far *)findbuf-temp);

		/* find the files:
			o	if an error occurred and it was not one of these listed, it's
				very bad news
			o	otherwise the error is no files found
			o	if no error, and we're not yet expanding a directory and we only
				found one file, and it has a directory attribute, try again
			o	otherwise, we found 1 or more files
		 */
		if(verbose)
			printf("\tSearching for %s, buffersize=%u attributes=0x%04x\n",
					filespec,bufsize,attributes);
		if(retval = DosFindFirst(filespec, &dirhandle, attributes, findbuf,
				bufsize, &found, 0L))
			{
			switch(retval)
				{
				case ERROR_FILE_NOT_FOUND:
				case ERROR_NO_MORE_FILES:
				case ERROR_PATH_NOT_FOUND:
					results->errorval = retval;
					break;
				default:
					error_exit(retval,"DosFindFirst");
				}
			results->firstfile = NULL;
			results->numfound = 0;
			}
		else
			{
			if(verbose)
				printf("\tFound %u files:\n",found);
			if(!dircheck &&  (found == 1) && (findbuf->attrFile & FILE_DIRECTORY)
					&&	!strcmp(findbuf->achName,
						&filespec[strlen(filespec)-strlen(findbuf->achName)]))
				{
				strcat(filespec,"\\*.*");
				dircheck = TRUE;
				DosSleep(32L);
				goto expand_directory;
				}
					
			results->firstfile = findbuf;
			results->numfound = found;
			request->totalresults += found;

			for( count = 0 ; found > 0; found--, count++)
				{
				if(verbose)
					{
					if(count == 5)
						{
						printf("\n");
						count = 0;
						}
					printf( ((findbuf->attrFile & FILE_DIRECTORY) ?
							"[%-12s] " : "%-15s"),findbuf->achName);
					}
				findbuf = (PFILEFINDBUF)(&(findbuf->cchName)+findbuf->cchName+2);
				}
			DosSleep(32L);
			}
		}

		/* now resize the segment to include only the data	*/
	request->resultsize = (USHORT)((char far *)findbuf-temp)+5;

	if(verbose)
		printf("\nThread #%u: Work area resized to %u bytes. Request complete.\n\n",
				threaddata->threadnum,request->resultsize);

	DosFreeSeg(request->serverwsel);					/* free the results segment*/
	DosSemClear(&request->RAMsem);					/* tell client we're done	*/
	threaddata->threadID = 0;
	DosExit(EXIT_THREAD,0);
	}

