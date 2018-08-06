/***	testprof.c - TestProfile
*
*	HISTORY:     4 May 87 - Initial version.
*
*/

#include <os2.h>
#include <stdio.h>
#include <conio.h>
#include "profile.h"

#define     OUTER_LOOP	    300     /* program does N*(N+1)/2 iterations */

/*  Check return code */
#define     crc(a)  doCrc((a),__FILE__,__LINE__)

doCrc (rc,file,line)
int	rc;
char   *file;
int	line;
{
    if (rc != 0) {
	printf ("%s:(%d) call failed; rc=%d\n", file, line, rc);
	exit (rc);
    }
}

main (argc,argv)
int argc;
char *argv[];
{
    long    i;
    unsigned    unused;

    if (argc != 2) {
	printf ("invalid parms\n");
	printf ("  usage: %s file\n", argv[0]);
	exit (1);
    }

    crc( PROFINIT(PT_USER|PT_USEKP, (char far *)NULL) );
    crc( PROFCLEAR(PT_USER) );
    crc( PROFON(PT_USER) );

    for (i=0; i<OUTER_LOOP; i++)
	inner(i);

    crc( PROFOFF(PT_USER) );
    crc( PROFDUMP(PT_USER, (char far *)argv[1]) );
    crc( PROFFREE(PT_USER) );
}

inner(i)
long i;
{
    long    j;
    long    k;

    for (j=0; j<i; j++)
	k=j;
}

