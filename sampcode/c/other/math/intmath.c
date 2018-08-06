/* INTMATH.C illustrates integer math functions including:
 *      abs         labs        min         max         div         ldiv
 *
 * See MATH.C for an example of the similar fabs function.
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

main()
{
    int x, y;
    long lx, ly;
    div_t divres;
    ldiv_t ldivres;

    printf( "Enter two integers: " );
    scanf( "%d %d", &x, &y );

    printf("Function\tResult\n\n" );
    printf( "abs\t\tThe absolute value of %d is %d\n", x, abs( x ) );
    printf( "min\t\tThe lesser of %d and %d is %d\n", x, y, min( x, y ) );
    printf( "max\t\tThe greater of %d and %d is %d\n", x, y, max( x, y ) );
    divres = div( x, y );
    printf( "div\t\tFor %d / %d, quotient is %d and remainder is %d\n\n",
            x, y, divres.quot, divres.rem );

    printf( "Enter two long integers: " );
    scanf( "%ld %ld", &lx, &ly );

    printf("Function\tResult\n\n" );
    ldivres = ldiv( lx, ly );
    printf( "labs\t\tThe absolute value of %ld is %ld\n", lx, labs( lx ) );
    printf( "ldiv\t\tFor %ld / %ld, quotient is %ld and remainder is %ld\n",
            lx, ly, ldivres.quot, ldivres.rem );
}
