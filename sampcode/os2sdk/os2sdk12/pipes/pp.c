/*
 * DosMakePipe example (pp.c, needs related program pc.c).
 *
 * Pipes are a byte stream oriented IPC mechanism which uses
 * standard DosRead and DosWrite file system calls. Since file
 * handles are used to access the pipe, each process using the
 * pipe needs to know the handles. There are two general ways
 * this is done:
 *
 *	1. Have very closely cooperating processes which notify
 *	   each other of the file handles via some private method
 *	   such as shared memory or command line arguments.
 *
 *	2. With unrelated processes, a process which reads and writes
 *	   from STDIN (handle 0) and STDOUT (handle 1) can be attached
 *	   to a pipe without being aware of it.
 *
 * This example demonstrates case #2 since that is the harder of the
 * two to set up. This process, pp.exe, creates a pipe and exec's a
 * child process pc.exe whose stdin is attached to the pipe. The parent
 * process writes a lower case message repeatedly into the pipe, and
 * the child process translates it into upper case and prints it on
 * the screen.
 *
 * Compile as: cl -AL -G2 -Lp pp.c   (parent process)
 *             cl -AL -G2 -Lp pc.c   (child process)
 *
 * Created by Microsoft Corp. 1987
 */
#define INCL_DOSQUEUES
#define INCL_DOSFILEMGR
#define INCL_DOSPROCESS

#include <os2def.h>
#include <bsedos.h>
#include <stdio.h>

#define PSIZE 256			/* pipe buffer size */

char msg[] = "hello there!\n";		/* message to write down pipe */
char exec_buf[100];			/* buffer for DosExecPgm ObjName */
char *pgmname = "pc.exe";		/* name of child program */

main()
{
	HFILE pread;			/* pipe read handle */
	HFILE pwrite;			/* pipe write handle */
	HFILE newstdin, newstdout;	/* stdin as a pipe handle */
	USHORT FileHandlState;		/* Used to modify handle inheritance */
	int i;
	USHORT bytecount;		/* bytes written result */
	RESULTCODES rc2;	       /* double return codes for EXEC */

	DosMakePipe(&pread, &pwrite, PSIZE);	/* create the pipe */

	/*
	 * Now close our stdin which is attached to the console,
	 * and make it refer to the pipe instead.
	 */
	DosClose(0);			/* close stdin */
	newstdin = 0;
	DosDupHandle(pread, &newstdin); /* make pipe = stdin */
	/*
	 * Since the child process will normaly inherit the handles of
	 * the parent we need to close the input of the pipe.  If we
	 * don't then the child will hang on the output of the pipe. (since
	 * it still has an open handle to the input).
	 *
	 * We don't really close the input, simply make it non-inheritable
	 * to the child.  Thus the only input to the pipe is from the parent.
	 * When the parent closes the pipe, any outstanding READs from the
	 * child will return a length of 0 (EOF).
	 */
	i = DosQFHandState(pwrite, &FileHandlState);
	if (i) printf("Query of pwrite failed\n");
	FileHandlState &= 0x7F88;	/* Mask bits offensive to the call */
	FileHandlState |= 0x080;	/* Deny inheritance to child */
	i = DosSetFHandState(pwrite, FileHandlState);
	if (i) printf("Set of pwrite failed, i = %x\n",i);
#ifdef NOCODE
	i = DosQFHandState(pread, &FileHandlState);
	if (i) printf("Query of pread failed\n");
	FileHandlState &= 0x7F88;	/* Mask bits offensive to the call */
	FileHandlState |= 0x080;	/* Deny inheritance to child */
	i = DosSetFHandState(pread, FileHandlState);
	if (i) printf("Set of pread failed, i = %x\n",i);
#endif
	/* exec child program, which will inherit new stdin */
	DosExecPgm(exec_buf, sizeof(exec_buf), EXEC_ASYNC, (PSZ)0L,
			(PSZ)0L, &rc2, pgmname);

	/* write 20 messages down the pipe */
	printf("Writing messages to the pipe\n");
	for(i = 0; i < 20; i++)
		DosWrite(pwrite, msg, sizeof(msg) - 1, &bytecount);

	DosClose(pwrite);
	DosClose(pread);
	printf("Parent exiting\n");
	DosExit(EXIT_PROCESS,0); /* Terminate, kill any lingering children */
}
