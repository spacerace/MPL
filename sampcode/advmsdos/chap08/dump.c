/*
    DUMP.C      Displays the binary contents of a file in
                hex and ASCII on the standard output device.

    Compile:    C>CL DUMP.C

    Usage:      C>DUMP unit:path\filename.ext  [ >device ]

    Copyright (C) 1988 Ray Duncan
*/

#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#define REC_SIZE 16             /* input file record size */

main(int argc, char *argv[])
{   
    int fd;                     /* input file handle */
    int status = 0;             /* status from file read */
    long fileptr = 0L;          /* current file byte offset */
    char filebuf[REC_SIZE];     /* data from file */

    if(argc != 2)               /* abort if missing filename */
    {   fprintf(stderr,"\ndump: wrong number of parameters\n");
        exit(1);
    }

                                /* open file in binary mode,
                                   abort if open fails */
    if((fd = open(argv[1],O_RDONLY | O_BINARY) ) == -1)
    {   fprintf(stderr, "\ndump: can't find file %s \n", argv[1]);
        exit(1);
    }

                                /* read and dump records 
                                   until end of file */
    while((status = read(fd,filebuf,REC_SIZE) ) != 0)
    {   dump_rec(filebuf, fileptr, status);
        fileptr += REC_SIZE;
    }

    close(fd);                  /* close input file */
    exit(0);                    /* return success code */
}


/*
    Display record (16 bytes) in hex and ASCII on standard output
*/

dump_rec(char *filebuf, long fileptr, int length)
{   
    int i;                      /* index to current record */

    if(fileptr % 128 == 0)      /* display heading if needed */
        printf("\n\n       0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");

    printf("\n%04lX ",fileptr); /* display file offset */

                                /* display hex equivalent of 
                                   each byte from file */
    for(i = 0; i < length; i++)
        printf(" %02X", (unsigned char) filebuf[i]);

    if(length != 16)            /* spaces if partial record */
        for (i=0; i<(16-length); i++) printf("   ");

                                /* display ASCII equivalent of 
                                   each byte from file */
    printf("  ");
    for(i = 0; i < length; i++)
    {   if(filebuf[i] < 32 || filebuf[i] > 126) putchar('.');
        else putchar(filebuf[i]);
    }
} 
