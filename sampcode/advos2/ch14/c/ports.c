/*
	PORTS.C

	An OS/2 IOPL demonstration program that reads and displays the
	first 256 I/O ports.  Requires the separate module PORTIO.ASM.

	Compile with:  C> cl /c ports.c
	Link with:  C> link ports+portio,ports,,os2,ports

	Usage is:  C> ports

	Copyright (C) 1988 Ray Duncan
*/

#include <stdio.h>

#define API extern far pascal

unsigned API rport(unsigned);           /* function prototypes */
void     API wport(unsigned, unsigned);
void     API DosSleep(unsigned long);
unsigned API DosPortAccess(unsigned, unsigned, unsigned, unsigned);

                                /* parameters for DosPortAccess */
#define REQUEST 0                       /* request port */
#define RELEASE 1                       /* release port */
#define BPORT   0                       /* beginning port */
#define EPORT   255                     /* ending port */

main(int argc, char *argv[])
{
    int i;                              /* scratch variable */

                                        /* request port access */
    if(DosPortAccess(0, REQUEST, BPORT, EPORT))
    {   
        printf("\nDosPortAccess failed.\n");
        exit(1);
    }

    printf("\n      ");                 /* print title line */
    for(i=0; i<16; i++) printf(" %2X", i);

    for(i=BPORT; i<=EPORT; i++)         /* loop through all ports */
    {
        if((i & 0x0f)==0)
        {
            printf("\n%04X  ", i);      /* new line needed */
        }

	printf(" %02X", rport(i));	/* read and display port */
    }
                                        /* release port access */
    DosPortAccess(0, RELEASE, BPORT, EPORT);
}


