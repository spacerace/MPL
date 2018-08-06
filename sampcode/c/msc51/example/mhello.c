/*
Multithread "Hello world!"
*/

#include <mt\malloc.h>
#include <mt\stdio.h>
#include <mt\stdlib.h>
#include <mt\io.h>
#include <mt\stddef.h>  /* definition of _threadid */

/* routines */
unsigned far pascal DosSleep ( long millisec ) ;
int main ( int argc , char * * argv ) ;
void childcode ( unsigned far * arg ) ;

/* global data */
char Result [ 32 ] ;
char buffer [ 16 ] ;
unsigned Synchronize ;

int main ( int argc , char * * argv )
{
    int rc ;
    unsigned result = 0 ;
    char * stackbottom ;
    long ChildCount ;
    int NumThreads ;
    int t ;
    int r ;
    int MaxThread = 0 ;
    long LoopCount ;

    if ( -- argc > 31 )
    {
    printf ( "*** Error: Too many arguments***\n" ) ;
    return (-1) ;
    }

    /* Bring up one thread for each argument */

    NumThreads = 0 ;

    while ( * ++ argv )
    {
        /* check for valid arguments */
       
        if (atol (* argv) == 0L)
        {
             printf ("*** Error: Non-integer argument ***\n") ;
             break ;
        }

        /* allocate a stack for the next thread */

        if ( ! ( stackbottom = _fmalloc ( 2048 ) ) )
        {
              printf ( "*** Error: Could not allocate a stack ***\n" ) ;
              break ;
        }

        /* bring up the new thread and pass the corresponding argument */

        ChildCount = atol ( * argv ) ;

        rc = _beginthread ( childcode , (void far *) stackbottom ,
              2048 , (void far *) ChildCount ) ;

        /* Check for error and keep track of how many threads we brought up */

        if ( rc == -1 || ChildCount == 0L)
        {
              printf ("*** Error: Could not Spawn %d-th Thread ***\n" ,
              NumThreads + 1) ;
              break ;
        }
        else
              NumThreads++;

        if ( rc > MaxThread )
              MaxThread = rc ;

    }

    /* Tell the user how many threads we brought up */

    printf ( "Number of threads = %d, Maximum thread ID = %d\n\n" ,
    NumThreads, MaxThread ) ;

    /* Let the threads begin execution  and wait for them to complete */

    LoopCount = 0L ;
    Synchronize = 1 ;

    for ( t = 0 ; t < NumThreads ; ++ t )
    {
        r = 0 ;
        while ( ! Result [ r ] )
        {
              DosSleep ( 0L ) ;
              if ( ++ r > MaxThread )
                   r = 0 ;
        }

        Result [ r ] = '\0' ;
    }

    printf("\nDone!\n");
    return 0 ;
}


/*
"Hello world!" per thread routine.
*/

void childcode ( unsigned far * arg )
{
    int tid = * _threadid ;
    int i;

    /* wait for the main routine to say "Go!" */
    while ( ! Synchronize )
        DosSleep ( 0L ) ;

    /* Print the message <arg> times */
    for (i=1; i<= (int)arg; i++)
        printf("Hello world from thread %i!\n", tid);

    /* Let the main program know we're done. */
    Result [ tid ] = (char) arg ;
}
