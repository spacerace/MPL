/* FCVT.C illustrates floating point to string conversion functions:
 *      gcvt            ecvt            fcvt
 *
 * See MKFPSTR.C for an example of using the data returned by fcvt
 * to build a formatted string. See ATONUM.C for an example of using
 * the string returned by gcvt.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

main()
{
    int decimal, sign;
    char *pnumstr;
    int precision = 7;
    char numbuf[50];
    double number1, number2;

    printf( "Enter two floating point numbers: " );
    scanf( "%lf %lf", &number1, &number2 );

    /* With gcvt, precision specifies total number of digits.
     * The decimal place and sign are inserted in the string.
     */
    gcvt( number1 + number2, precision, numbuf );
    printf( "\nString produced by gcvt: %s\n", numbuf );
    printf( "Total digits: %d\n", precision );

    /* With ecvt, precision specifies total number of digits.
     * The decimal place and sign are provided for use in formatting.
     */
    pnumstr = ecvt( number1 + number2, precision, &decimal, &sign );
    printf( "\nString produced by ecvt: %s\nSign: %s\n",
             pnumstr, sign ? "-" : "+" );
    printf( "Digits left of decimal: %d\nTotal digits: %d\n",
             decimal, precision );

    /* With fcvt, precision specifies digits after decimal place.
     * The decimal place and sign are provided for use in formatting.
     */
    pnumstr = fcvt( number1 + number2, precision, &decimal, &sign );
    printf( "\nString produced by fcvt: %s\nSign: %s\n",
             pnumstr, sign ? "-" : "+"  );
    printf( "Digits left of decimal: %d\nDigits after decimal: %d\n",
             decimal, precision );
}
