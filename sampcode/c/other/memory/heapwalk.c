/* HEAPWALK.C illustrates heap testing functions including:
 *      _heapchk        _fheapchk       _nheapchk
 *      _heapset        _fheapset       _nheapset
 *      _heapwalk       _fheapwalk      _nheapwalk
 *      _msize          _fmsize         _nmsize
 *
 * Only the model independent versions are shown. They map to the model
 * dependent versions, depending on the memory model.
 */

#include <stdio.h>
#include <conio.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>

/* Macro to get a random integer within a specified range */
#define getrandom( min, max ) ((rand() % (int)((max) - (min))) + (min) + 1)

void heapdump( char fill );
void heapstat( int status );

main()
{
    int *p[10], i;

    srand( (unsigned)time( 0L ) );  /* Seed with current time. */

    /* Check heap status. Should be OK at start of heap. */
    heapstat( _heapchk() );

    /* Now do some operations that affect the heap. In this example,
     * allocate random-size blocks.
     */
    for( i = 0; i < 10; i++ )
    {
        if( (p[i] = (int *)calloc( getrandom( 1, 10000 ),
                                   sizeof( int ) )) == NULL )
        {
            --i;
            break;
        }
        printf( "Allocated %u at %p\n", _msize( p[i] ), (void far *)p[i] );
    }

    /* Fill all free blocks with the test character. */
    heapstat( _heapset( 254 ) );

    /* In a real program, you might do operations here on the allocated
     * buffers. Then do heapdump to make sure none of the operations wrote
     * to free blocks.
     */
    heapdump( 254 );

    /* Do some more heap operations. */
    for( ; i >= 0; i-- )
    {
        free( p[i] );
        printf( "Deallocating %u at %p\n", _msize( p[i] ), (void far *)p[i] );
    }

    /* Check heap again. */
    heapdump( 254 );
}

/* Test routine to check each block in the heap. */
void heapdump( char fill )
{
    struct _heapinfo hi;
    int heapstatus, i;
    char far *p;

    /* Walk through entries, displaying results and checking free blocks. */
    printf( "\nHeap dump:\n" );
    hi._pentry = NULL;
    while( (heapstatus = _heapwalk( &hi )) == _HEAPOK )
    {
        printf( "\n\t%s block at %p of size %u\t",
                hi._useflag == _USEDENTRY ? "USED" : "FREE",
                hi._pentry, hi._size );

        /* For free entries, check each byte to see that it still has
         * only the fill character.
         */
        if( hi._useflag != _USEDENTRY )
        {
            for( p = (char far *)hi._pentry, i = 0; i < hi._size; p++, i++ )
                if( (char)*p != fill )
                    break;
            if( i == hi._size )
                printf( "Not changed" );
            else
                printf( "Changed" );
        }
    }
    heapstat( heapstatus );
}

/* Report on the status returned by _heapwalk, _heapset, or _heapchk. */
void heapstat( int status )
{
    printf( "\nHeap status: " );
    switch( status )
    {
        case _HEAPOK:
            printf( "OK - heap is fine" );
            break;
        case _HEAPEMPTY:
            printf( "OK - empty heap" );
            break;
        case _HEAPEND:
            printf( "OK - end of heap" );
            break;
        case _HEAPBADPTR:
            printf( "ERROR - bad pointer to heap" );
            break;
        case _HEAPBADBEGIN:
            printf( "ERROR - bad start of heap" );
            break;
        case _HEAPBADNODE:
            printf( "ERROR - bad node in heap" );
            break;
    }
    printf( "\n\n" );
}
