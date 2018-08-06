/*
	PROTO.C

	A filter template for OS/2.

	Compile with:  C> cl proto.c

	Usage is:  C> proto <source >destination

	Copyright (C) 1988 Ray Duncan
*/

#include <stdio.h>

#define STDIN   0                   /* standard input handle */
#define STDOUT  1                   /* standard output handle */
#define BUFSIZE 256                 /* I/O buffer size */

#define API unsigned extern far pascal 

API DosRead(unsigned, void far *, unsigned, unsigned far *);
API DosWrite(unsigned, void far *, unsigned, unsigned far *);

static char input[BUFSIZE];         /* buffer for input line */
static char output[BUFSIZE];        /* buffer for output line */

main(int argc,char *argv[])
{       
    int rlen, wlen;                 /* scratch variables */

    while(1)                        /* do until end of file */
    {       
                                    /* get line from standard
                                       input stream */
        if(DosRead(STDIN, input, BUFSIZE, &rlen)) 
	    exit(1);		    /* exit if read error */

        if(rlen == 0) exit(0);      /* exit if end of stream */

                                    /* write translated line to 
                                       standard output stream */
        if(DosWrite(STDOUT, output, translate(rlen), &wlen)) 
            exit(1);                /* exit if write error */
    }
}

/*
    Perform any necessary translation on input line, 
    leaving the resulting text in output buffer.  
    Returns length of translated line (may be zero).
*/

int translate(int length)
{       
    memcpy(output,input,length);    /* template action is */
                                    /* to copy input line */
    return(length);                 /* and return its length */
}


