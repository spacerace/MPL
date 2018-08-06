/* MKFPSTR.C illustrates building and displaying floating point strings
 * without printf. Functions illustrated include:
 *      strcat          strncat         cscanf          fcvt
 */

#include <stdlib.h>
#include <conio.h>
#include <string.h>

main()
{
    int decimal, sign;
    int precision = 7;
    static char numbuf[50] = ""; 
    char *pnumstr, tmpbuf[50];
    double number1, number2;

    cputs( "Enter two floating point numbers: " );
    cscanf( "%lf %lf", &number1, &number2 );
    putch( '\n' );

    /* Use information from fcvt to format number string. */
    pnumstr = fcvt( number1 + number2, precision, &decimal, &sign );

    /* Start with sign if negative. */
    if( sign )
        strcat( numbuf, "-" );

    if( decimal <= 0 )
    {
        /* If decimal is left of first digit (decimal negative), put
         * in leading zeros, then add digits.
         */
        strcat( numbuf, "0." );
        memset( tmpbuf, '0', (size_t)abs( decimal ) );
        tmpbuf[abs( decimal )] = 0;
        strcat( numbuf, tmpbuf );
        strcat( numbuf, pnumstr );
    }
    else
    {
        /* If decimal is right of first digit, put in leading digits.
         * then add decimal and trailing digits.
         */
        strncat( numbuf, pnumstr, (size_t)decimal );
        strcat( numbuf, "." );
        strcat( numbuf, pnumstr + decimal );
    }
    cputs( strcat( "Total is: ", numbuf ) );
}
