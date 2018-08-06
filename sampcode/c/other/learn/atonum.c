/* ATONUM.C illustrates string to number conversion functions including:
 *      atof            atoi            atol            gcvt
 *
 * It also illustrates:
 *      cgets           cputs
 */

#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define MAXSTR 100

char cnumbuf[MAXSTR] = { MAXSTR + 2, 0 };
#define numbuf cnumbuf + 2          /* Actual buffer starts at byte 3 */
char tmpbuf[MAXSTR];

/* Numeric input and output without printf. */
main()
{
    int     integer;
    long    longint;
    float   real;

    /* Using cgets (rather than gets) allows use of DOS editing keys
     * (or of editing keys from DOS command line editors).
     */
    cputs( "Enter an integer: " );
    cgets( cnumbuf );
    cputs( "\n\r" );                /* cputs doesn't translate \n     */
    integer = atoi( numbuf );
    strcpy( tmpbuf, numbuf );
    strcat( tmpbuf, " + " );

    cputs( "Enter a long integer: " );
    cgets( cnumbuf );
    cputs( "\n\r" );
    longint = atol( numbuf );
    strcat( tmpbuf, numbuf );
    strcat( tmpbuf, " + " );

    cputs( "Enter a floating point number: " );
    cgets( cnumbuf );
    cputs( "\n\r" );
    real = atof( numbuf );
    strcat( tmpbuf, numbuf );
    strcat( tmpbuf, " = " );

    gcvt( integer + longint + real, 4, numbuf );
    strcat( tmpbuf, numbuf );
    strcat( tmpbuf, "\n\r" );

    cputs( tmpbuf );
}
