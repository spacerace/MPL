/* STRTONUM.C illustrates string to number conversion functions including:
 *      strtod              strtol              strtoul
 */

#include <stdlib.h>
#include <stdio.h>

main()
{
    char *string, *stopstring;
    double x;
    long l;
    unsigned long ul;
    int base;

    /* Convert string to double. */
    string = "3.1415926INVALID";
    x = strtod( string, &stopstring );
    printf( "\nString: %s\n", string );
    printf( "\tDouble:\t\t\t%f\n", x );
    printf( "\tScan stopped at:\t%s\n", stopstring );

    /* Convert string to long using bases 2, 4, and 8. */
    string = "-10110134932";
    printf( "\nString: %s\n", string );
    for( base = 2; base <= 8; base *= 2 )
    {
        l = strtol( string, &stopstring, base );
        printf( "\tBase %d signed long:\t%ld\n", base, l );
        printf( "\tScan stopped at:\t%s\n", stopstring );
    }

    /* Convert string to unsigned long using bases 2, 4, and 8. */
    string = "10110134932";
    printf( "\nString: %s\n", string );
    for( base = 2; base <= 8; base *= 2 )
    {
        ul = strtoul( string, &stopstring, base);
        printf("\tBase %d unsigned long:\t%ld\n", base, ul );
        printf("\tScan stopped at:\t%s\n", stopstring );
    }
}
