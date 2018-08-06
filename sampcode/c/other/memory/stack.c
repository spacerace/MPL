/* STACK.C illustrates stack functions including:
 *      alloca          stackavail
 */

#include <malloc.h>
#include <stdio.h>

main()
{
    char *buffer;

    printf( "Bytes available on stack: %u\n", stackavail() );

    /* Allocate memory for string. */
    buffer = alloca( 120 * sizeof( char ) );
    printf( "Enter a string: " );
    gets( buffer );
    printf( "You entered: %s\n", buffer );

    printf( "Bytest available on stack: %u\n", stackavail() );
}
