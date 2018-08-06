/* TRIG.C illustrates trignometric functions including:
 *    cos          cosh           acos
 *    sin          sinh           asin
 *    tan          tanh           atan          atan2
 */

#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

main()
{
    double x, rx, y;

    do
    {
        printf( "\nEnter a real number between 1 and -1: " );
        scanf( "%lf", &x );
    } while( (x > 1.0) || (x < -1.0) );

    printf("\nFunction\tResult for %2.2f\n\n", x );
    if( (x <= 1.0) && (x >= -1.0) )
    {
        printf( "acos\t\t%2.2f\n", acos( x ) );
        printf( "asin\t\t%2.2f\n", asin( x ) );
    }
    if( (rx = cos( x )) && (errno != ERANGE) )
        printf( "cos\t\t%2.2f\n", rx );
    else
        errno = 0;
    if( (rx = sin( x )) && (errno != ERANGE) )
        printf( "sin\t\t%2.2f\n", rx );
    else
        errno = 0;
    if( (rx = cosh( x )) && (errno != ERANGE) )
        printf( "cosh\t\t%2.2f\n", rx );
    else
        errno = 0;
    if( (rx = sinh( x )) && (errno != ERANGE) )
        printf( "sinh\t\t%2.2f\n", rx );
    else
        errno = 0;

    printf( "\nEnter a real number of any size: " );
    scanf( "%lf", &x );
    printf("\nFunction\tResult for %2.2f\n\n", x );
    printf( "atan\t\t%2.2f\n", atan( x ) );
    if( (rx = tan( x )) && (errno != ERANGE) )
        printf( "tan\t\t%2.2f\n", rx );
    else
        errno = 0;
    printf( "tanh\t\t%2.2f\n", tanh( x ) );

    printf( "\nEnter another real number of any size: " );
    scanf( "%lf", &y );
    printf("\nFunction\tResult for %2.2f and %2.2f\n\n", x, y );
    if( rx = atan2( x, y ) )
        printf( "atan2\t\t%2.2f\n", rx );
    else
        errno = 0;
}
