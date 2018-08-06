/* SORT.C illustates randomizing, sorting, and searching. Functions
 * illustrated include:
 *      srand           rand            qsort
 *      lfind           lsearch         bsearch
 *
 * The lsearch function is not specifically shown in the program, but
 * its use is the same as lfind except that if it does not find the
 * element, it inserts it at the end of the array rather than failing.
 */

#include <search.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define  ASIZE 1000
unsigned array[ASIZE];

/* Macro to get a random integer within a specified range */
#define getrandom( min, max ) ((rand() % (int)((max) - (min))) + (min) + 1)

/* Must be declared before call */
unsigned cmpgle( unsigned *arg1, unsigned *arg2 );
unsigned cmpe( unsigned *arg1, unsigned *arg2 );

main()
{
    unsigned i, *result, elements = ASIZE, key = ASIZE / 2;

    /* Seed the random number generator with current time. */
    srand( (unsigned)time( 0L ) );

    /* Build a random array of integers. */
    printf( "Randomizing . . .\n" );
    for( i = 0; i < ASIZE; i++ )
        array[i] = getrandom( 1, ASIZE );

    /* Show every tenth element. */
    printf( "Printing every tenth element . . .\n" );
    for( i = 9; i < ASIZE; i += 10 )
    {
        printf( "%5u", array[i] );
        if( !(i % 15) )
            printf( "\n" );
    }

    /* Find element using linear search. */
    printf( "\nDoing linear search . . .\n" );
    result = (unsigned *)lfind( (char *)&key, (char *)array, &elements,
                                sizeof( unsigned ), cmpe);
    if( result == NULL )
        printf( "  Value %u not found\n", key );
    else
        printf( "  Value %u found in element %u\n", key, result - array + 1);

    /* Sort array using Quicksort algorithm. */
    printf( "Sorting . . .\n" );
    qsort( (void *)array, (size_t)ASIZE, sizeof( int ), cmpgle );

    /* Show every tenth element. */
    printf( "Printing every tenth element . . .\n" );
    for( i = 9; i < ASIZE; i += 10 )
    {
        printf( "%5u", array[i] );
        if( !(i % 15) )
            printf( "\n" );
    }

    /* Find element using binary search. Note that the array must
     * be previously sorted before using binary search.
     */
    printf( "\nDoing binary search . . .\n" );
    result = (unsigned *)bsearch( (char *)&key, (char *)array, elements,
                                  sizeof( unsigned ), cmpgle);
    if( result == NULL )
        printf( "  Value %u not found\n", key );
    else
        printf( "  Value %u found in element %u\n", key, result - array + 1 );

}

/* Compare and return greater than (1), less than (-1), or equal to (0).
 * This function is called by qsort and bsearch.
 */
unsigned cmpgle( unsigned *arg1, unsigned *arg2 )
{
    if( *arg1 > *arg2 )
        return 1;
    else if( *arg1 < *arg2 )
        return -1;
    else
        return 0;
}

/* Compare and return equal (1) or not equal (0). This function is
 * called by lfind and lsearch.
 */
unsigned cmpe( unsigned *arg1, unsigned *arg2 )
{
    return !( *arg1 == *arg2 );
}
