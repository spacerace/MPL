/***	pchild.c - child program
 *
 *	Example of DosMakePipe usage in parent/child communication
 *
 *	This is the child program which read the data sent down
 *	from the parent in the pipe.
 *
 * Created by Microsoft Corp. 1987
 */
#define INCL_DOSFILEMGR
#define INCL_DOSMEMMGR

#include <os2def.h>
#include <stdio.h>
#include <bsedos.h>

typedef struct {
	SHANDLE read_handle;	       /* pipe read handle */
	SHANDLE write_handle;	       /* pipe write handle */
} SharedData;


main()
{

	static char pname[] = "\\SHAREMEM\\public";   /* shared mem seg name */
	char *written = "Writing to the child";	      /* string in pipe */
	char readin[21];		/* DosRead input buffer */
	int retcode;			/* holds return code from call */
	SharedData far *fp;		/* pointer to shared memory */
	SEL mem_handle; 		/* selector of the allocated segment */
	USHORT buflen = 21;		/* DosRead buffer length */
	USHORT read;			/* number bytes read by DosRead */


	/* access shared memory 'public' */
	printf("Accessing shared memory\n");
	retcode = DosGetShrSeg((PSZ)pname, &mem_handle);
	
	/* create pointer to shared memory segment */
	fp = (SharedData far *)MAKEP(mem_handle,0);

        /* read from the pipe */
	printf("Reading from pipe\n");
	if( retcode = DosRead( fp->read_handle, readin, buflen,
			&read)) {
                printf("Read from pipe handle %d failed, retcode %d\n", 
		    fp->read_handle, retcode);
        }
	else {
		printf("DosRead read %d bytes from handle %d, retcode %d\n", 
		    read, fp->read_handle, retcode);

		/* verify the string */
		if ( retcode = strcmp( written, readin ) ) {
			printf("The child didn't read pipe data correctly");
			printf(", retcode %d\n", retcode);
			printf("read string %s\n", readin);
			printf("expected string %s\n", written);
		}
		else
			printf("Read pipe data ok\n");
	}

	/* free the segment and return to parent */
	DosFreeSeg( mem_handle );

	printf("Exiting child\n");

	/* Exit without terminating other children */

	DosExit(EXIT_THREAD,0);
}
