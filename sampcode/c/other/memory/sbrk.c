/* SBRK.C illustrates memory allocation with function:
 *      sbrk
 */

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

main()
{
    char *bufA, *bufB, *bufC;

    /* Allocate 512 bytes. */
    if( (bufA = sbrk( 512 )) == (char *)-1 )
        exit( 1 );
    printf( "Allocate:\t256\tLocation:\t%p\n", (int far *)bufA );

    /* Allocate 50 bytes. */
    if( (bufB = sbrk( 50 )) == (char *)-1 )
        exit( 1 );
    printf( "Allocate:\t50\tLocation:\t%p\n", (int far *)bufB );

    /* Deallocate 256 bytes. */
    sbrk( -256 );
    printf( "Deallocate:\t256\n" );

    /* Allocate 20 bytes. */
    if( (bufC = sbrk( 20 )) == (char *)-1 )
        exit( 1 );
    printf( "Allocate:\t20\tLocation:\t%p\n", (int far *)bufC );
    exit( 0 );
}
