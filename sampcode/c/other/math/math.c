/* MATH.C illustrates floating point math functions including:
 *      exp             pow             sqrt            frexp
 *      log             log10           ldexp           modf
 *      ceil            floor           fabs            fmod
 */

#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

main()
{
    double x, rx, y;
    int n;

    printf( "\nEnter a real number: " );
    scanf( "%lf", &x );

    printf( "Mantissa: %2.2lf\tExponent: %d\n", frexp( x, &n ), n );
    printf( "Fraction: %2.2lf\tInteger: %lf\n", modf( x, &y ), y );

    printf("\nFunction\tResult for %2.2f\n\n", x );
    if( (rx = exp( x )) && (errno != ERANGE) )
        printf( "exp\t\t%2.2f\n", rx );
    else
        errno = 0;
    if( x > 0.0 )
        printf( "log\t\t%2.2f\n", log( x ) );
    if( x > 0.0 )
        printf( "log10\t\t%2.2f\n", log10( x ) );
    if( x >= 0.0 )
        printf( "sqrt\t\t%2.2f\n", sqrt( x ) );
    printf( "ceil\t\t%2.2f\n", ceil( x ) );
    printf( "floor\t\t%2.2f\n", floor( x ) );
    printf( "fabs\t\t%2.2f\n", fabs( x ) );

    printf( "\nEnter another real number: " );
    scanf( "%lf", &y );
    printf("\nFunction\tResult for %2.2f and %2.2f\n\n", x, y );
    printf( "fmod\t\t%2.2f\n", fmod( x, y ) );
    rx = pow( x, y );
    if( (errno != ERANGE) && (errno != EDOM) )
        printf( "pow\t\t%2.2f\n", rx );
    else
        errno = 0;
    rx = hypot( x, y );
    if( errno != ERANGE )
        printf( "hypot\t\t%2.2f\n", hypot( x, y ) );
    else
        errno = 0;

    printf( "\nEnter an integer exponent: " );
    scanf( "%d", &n );
    rx = ldexp( x, n );
    if( errno != ERANGE )
    {
        printf("\nFunction\tResult for %2.2f to power %d\n\n", x, n );
        printf( "ldexp\t\t%2.2f\n", ldexp( x, n ) );
    }
}
