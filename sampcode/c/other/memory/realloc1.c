/* REALLOC.C illustrates heap functions:
 *      calloc      realloc         _expand
 */

#include <stdio.h>
#include <malloc.h>

main()
{
    int  *bufint;
    char *bufchar;

    printf( "Allocate two 512 element buffers\n" );
    if( (bufint = (int *)calloc( 512, sizeof( int ) )) == NULL )
        exit( 1 );
    printf( "Allocated %d bytes at %Fp\n",
            _msize( bufint ), (void far *)bufint );

    if( (bufchar = (char *)calloc( 512, sizeof( char ) )) == NULL )
        exit( 1 );
    printf( "Allocated %d bytes at %Fp\n",
            _msize( bufchar ), (void far *)bufchar );

    /* Expand the second buffer, reallocate the first. Note that trying
     * to expand the first buffer would fail because the second buffer
     * would be in the way.
     */
    if( (bufchar = (char *)_expand( bufchar, 1024 )) == NULL )
        printf( "Can't expand" );
    else
        printf( "Expanded block to %d bytes at %Fp\n",
                _msize( bufchar ), (void far *)bufchar );

    if( (bufint = (int *)realloc( bufint, 1024 * sizeof( int ) )) == NULL )
        printf( "Can't reallocate" );
    else
        printf( "Reallocated block to %d bytes at %Fp\n",
                _msize( bufint ), (void far *)bufint );

    /* Free memory */
    free( bufint );
    free( bufchar );
    exit( 0 );
}
