/*
	DUMP.C

	Displays the binary contents of a file in hex and ASCII on
	the standard output device.  Demonstrates direct calls to
	OS/2 API from a C program.

	Compile with:  C> cl dump.c

	Usage is:  C> dump pathname.ext [>destination]

	Copyright (C) 1987 Ray Duncan
*/

#include <stdio.h>

#define RECSIZE 16                  /* size of file records */

#define API unsigned extern far pascal 

API DosClose(unsigned);             /* function prototypes */

API DosOpen(char far *, unsigned far *, unsigned far *, unsigned long,
            unsigned, unsigned, unsigned, unsigned long);           

API DosRead(unsigned, void far *, unsigned, unsigned far *);

main(int argc, char *argv[])
{   
    char file_buf[RECSIZE];         /* data block from file */
    unsigned long foffset = 0L;     /* file offset in bytes */
    unsigned handle;                /* DosOpen variable */
    unsigned action, length;        /* DosRead variables */
    unsigned flag = 0x01;           /* fail if file not found */
    unsigned mode = 0x40;           /* read only, deny none */

    if(argc < 2)                    /* check command tail */
    {   
        fprintf(stderr, "\ndump: missing file name\n");
        exit(1);
    }
                                    /* open file or exit */
    if(DosOpen(argv[1], &handle, &action, 0L, 0, flag, mode, 0L)) 
    {   
        fprintf(stderr, "\ndump: can't find file %s\n", argv[1]);
        exit(1);
    }  
                                    /* read and dump records */
    while((DosRead(handle, file_buf, RECSIZE, &length) == 0) 
           && (length != 0))
    {   
        dump_rec(file_buf, foffset, length);
        foffset += RECSIZE;
    }
    printf("\n");                   /* extra blank line */
    DosClose(handle);               /* close the input file */
    exit(0);                        /* return success code */
}

/*
    Display record (16 bytes) in hex and ASCII on standard output.
*/
dump_rec(char *buffer, long foffset, int length)
{   
    int i;                          /* index to current record */

    if(foffset % 128 == 0)          /* maybe print heading */
        printf("\n\n       0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");
        
    printf("\n%04lX ", foffset);    /* file offset */

    for(i = 0; i < length; i++)     /* print hex equiv. of each byte */
        printf(" %02X", (unsigned char) buffer[i]);

    if(length != 16)                /* space over if last rec. */
        for(i=0; i<(16-length); i++) printf("   ");

    printf("  ");
    for(i = 0; i < length; i++)     /* print ASCII equiv. of bytes */
    {   
        if(buffer[i] < 32 || buffer[i] > 126) putchar('.');
        else putchar(buffer[i]);
    }
} 

