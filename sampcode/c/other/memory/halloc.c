/* HALLOC.C illustrates dynamic allocation of huge memory using functions:
 *      halloc          hfree
 */

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

main()
{
    char huge *bigbuf, huge *p;
    long count = 100000L;

    /* Allocate huge buffer. */
    bigbuf = (char huge *)halloc( count, sizeof( char ) );
    if( bigbuf == NULL )
    {
        printf( "Insufficient memory" );
        exit( 1 );
    }

    /* Fill the buffer with characters. */
    for( p = bigbuf; count; count--, p++ )
        *p = (char)(count % 10) + '0';

    /* Free huge buffer. */
    hfree( bigbuf );
    exit( 0 );
}
