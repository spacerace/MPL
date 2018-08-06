/* FREECT.C illustrates the following heap functions:
 *      _freect         _memavl
 */

#include <malloc.h>
#include <stdio.h>

main()
{
    char near *bufs[64];
    unsigned request, avail, i;

    printf( "Near heap bytes free: %u\n\n", _memavl() );
    printf( "How many 1K buffers do you want from the near heap? " );
    scanf( "%d", &request );
    if( request > 64 )
    {
        printf( "There are only 64K in a segment.\n" );
        request = 64;
    }

    avail = _freect( 1024 );
    request = (avail > request) ? request : avail;
    printf( "You can have %d buffers\n", request );

    printf( "They are available at:\n");
    for( i = 0; i < request; i++ )
    {
        bufs[i] = (char near *)_nmalloc( 1024 );
        printf( "%2d %Fp   ", i + 1, (char far *)bufs[i] );
        if( (i % 5) == 4 )
            printf( "\n" );
    }
    printf( "\n\nNear heap bytes free: %u\n\n", _memavl() );
    printf( "Freeing buffers . . ." );
    for( i = request; i; i-- )
        _nfree( bufs[i] );
    printf( "\n\nNear heap bytes free: %u", _memavl() );
}
