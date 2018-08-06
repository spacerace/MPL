/* ATEXIT.C illustrates:
 *      atexit          onexit
 */

#include <stdlib.h>
#include <stdio.h>
#define ANSI                /* Comment out to try onexit     */

/* Prototypes */
void fn1( void ), fn2( void ), fn3( void ), fn4( void );

main()
{

    /* atexit is ANSI standard. It returns 0 for success, nonzero
     * for fail.
     */
#ifdef ANSI
    atexit( fn1 );
    atexit( fn2 );
    atexit( fn3 );
    atexit( fn4 );

    /* onexit is Microsoft extension. It returns pointer to function
     * for success, NULL for fail.
     */
#else
    onexit( fn1 );
    onexit( fn2 );
    onexit( fn3 );
    onexit( fn4 );
#endif

    printf( "This is executed first.\n" );
}

void fn1()
{
    printf( "next.\n" );
}

void fn2()
{
    printf( "executed " );
}

void fn3()
{
    printf( "is " );
}

void fn4()
{
    printf( "This " );
}
