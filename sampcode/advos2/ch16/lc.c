/*
    LC.C    Filter to translate uppercase {A-Z} 
            to lowercase {a-z} characters.
    Copyright (C) 1988 Ray Duncan

    Compile:    CL lc.c

    Usage:      LC  [<source]  [>destination]
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

main(int argc,char argv)
{       
    int rlen,wlen;                  /* scratch variables */

    while(1)                        /* do until end of file */
    {                               /* get line from standard input */
        DosRead(STDIN,input,BUFSIZE,&rlen);
        
        if(rlen == 0) exit(0);      /* exit if end of stream */

                                    /* write translated line
                                       to standard output */
        DosWrite(STDOUT,output,translate(rlen),&wlen);
    }
}


/*
    Translate upper case characters to lower
    case characters, leaving resulting text
    in output buffer and returning its length.
*/
int translate(int length)
{       
    int i;                          /* scratch variable */

    memcpy(output,input,length);    /* copy input to output */

    for(i = 0; i < length; i++)     /* lowercase the output */
    {       
        if(output[i] >= 'A' && output[i] <= 'Z') 
            output[i] += 'a'-'A';
    }
    return(length);                 /* and return its length */
}

