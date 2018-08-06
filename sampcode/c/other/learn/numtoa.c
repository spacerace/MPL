/* NUMTOA.C illustrates number to string conversion functions including:
 *      itoa            ltoa            ultoa
 */

#include <stdlib.h>
#include <stdio.h>

main()
{
    int  base, i;
    long l;
    unsigned long ul;
    char buffer[60];

    printf( "Enter an integer: " );
    scanf( "%d", &i );
    for( base =  2; base <= 16; base += 2 )
    {
        itoa( i, buffer, base );
        if( base != 10 )
            printf( "%d in base %d is: %s\n", i, base, buffer );
    }

    printf( "Enter a long integer: " );
    scanf( "%ld", &l );
    for( base =  2; base <= 16; base += 2 )
    {
        ltoa( l, buffer, base );
        if( base != 10 )
            printf( "%ld in base %d is: %s\n", l, base, buffer );
    }

    printf( "Enter an unsigned long integer: " );
    scanf( "%lu", &ul );
    for( base =  2; base <= 16; base += 2 )
    {
        ultoa( ul, buffer, base );
        if( base != 10 )
            printf( "%lu in base %d is: %s\n", ul, base, buffer );
    }
}
