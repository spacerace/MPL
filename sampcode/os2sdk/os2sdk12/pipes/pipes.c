/***	pipes.c - parent program
 *
 *	Example of DosMakePipe usage in parent/child communication
 *
 *	This program gets some shared memory, makes a pipe, writes
 *	a string into it, and then execs a child.  The child gets
 *	the shared memory segment, then reads from the pipe using
 *	the handle passed in the shared memory segment.
 *
 * Created by Microsoft Corp. 1987
 */

#define INCL_DOSPROCESS
#define INCL_DOSFILEMGR
#define INCL_DOSMEMMGR
#define INCL_DOSQUEUES

#include <os2def.h>
#include <stdio.h>
#include <bsedos.h>

typedef struct {			/* structure of shared mem segment */
	SHANDLE  read_handle;		/* pipe read handle */
	SHANDLE  write_handle;		/* pipe write handle */
} SharedData;

SharedData far *fp;			/* pointer to shared memory */
SEL mem_handle; 			/* selector of the allocated segment */


main()
{

	static char pname[] = "\\SHAREMEM\\public";	   /* shared mem name */
	static char writeout[] = "Writing to the child";   /* pipe string */
	char exec_buf[100];		/* buffer for DosExecPgm ObjName */
	char *pgmname = "pchild.exe";	/* name of child program */
	int i;
	int retcode;			/* holds return code from call */
	RESULTCODES	   tcodes;	/* termination codes from DosExecPgm */
	unsigned far *fpinit;		/* pointer to shared memory */
	USHORT buflen;			/* DosWrite buffer length */
	USHORT memsiz = 1024;		/* size of memory segment requested */
	USHORT pipe_size;		/* size to reserve for the pipe */
	USHORT written; 		/* number bytes written by DosWrite */

	/* allocate 1k shared memory segment and initialize it */
	printf("Getting shared memory segment\n");
	retcode = DosAllocShrSeg( memsiz, (PSZ)pname,
			(PSEL)&mem_handle);

	/* create pointers to shared memory segment */
	fp = (SharedData far *)MAKEP(mem_handle,0);
	fpinit = (unsigned far *)MAKEP(mem_handle,0);

	/* zero initialize the segment */
	for (i = 0; i < memsiz / 2; i++) {
	    *fpinit = 0;
	    fpinit++;
	}

	/* make the pipe */
	pipe_size = 0;			/* use default size */
	printf("Making the pipe\n");
	retcode = DosMakePipe((PHFILE)&fp->read_handle,
			(PHFILE)&fp->write_handle, pipe_size);

	if ( retcode ) {
	    printf("DosMakePipe returned error %d, aborting\n", retcode);
	    exit(2);
	}
	else
	    printf("DosMakePipe retcode %d, read_handle %d, write_handle %d\n",
		retcode, fp->read_handle, fp->write_handle);

        /* write string to pipe */
	buflen = strlen(writeout) + 1;
	if( retcode = DosWrite( fp->write_handle, (PCHAR)writeout,
			buflen, &written)) {
                printf("Write to pipe failed, retcode %d\n", retcode);
                exit(2);
        }
	else 
		printf("Write to write_handle %d, bytes written %d\n",
		    fp->write_handle, written);
        
        /* create the child */
	printf("Creating child\n");
	retcode = DosExecPgm( exec_buf, 100, EXEC_SYNC,
			(PSZ)0L, (PSZ)0L,
			&tcodes, pgmname );

	if ( retcode )
                printf("DosExecPgm of child error %d\n", retcode);
	else 
		printf("Back in parent\n");
	
	/* close the pipe */
	DosClose( fp->read_handle );
	DosClose( fp->write_handle );

	DosExit(EXIT_PROCESS,0);   /* Terminate, kill any dangling children */

}
