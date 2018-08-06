/* SIGNAL.C illustrates setting up signal interrupt routines. Functions
 * illustrated include:
 *      signal          abort           raise           _fpreset
 *
 * The program also illustrates saving an environment and then doing a
 * long jump to return to it using functions:
 *      setjmp          longjmp
 */

#include <stdio.h>
#include <conio.h>
#include <signal.h>
#include <process.h>
#include <setjmp.h>
#include <stdlib.h>
#include <float.h>

void ctrlchandler( void );          /* Prototypes */
void aborthandler( void );
void fphandler( int sig, int num );

jmp_buf mark;                       /* Address for long jump to jump to */

main()
{
    double n1, n2, r;
    int jmpret;

    /* Modify abort behavior. */
    if( signal( SIGABRT, aborthandler ) == SIG_ERR )
    {
        printf( "Couldn't set SIGABRT\n" );
        exit( 1 );
    }

    /* Modify CTRL+C behavior. */
    if( signal( SIGINT, ctrlchandler ) == SIG_ERR )
    {
        fprintf( stderr, "Couldn't set SIGINT\n" );
        abort();
    }

    /* Set up pointer to error handler. */
    if( signal( SIGFPE, fphandler ) == SIG_ERR )
    {
        fprintf( stderr, "Couldn't set SIGFPE\n" );
        abort();
    }

    /* Save stack environment for return in case of error. First time
     * through, jmpret is 0, so true conditional is executed. If an
     * error occurs, jmpret will be set to -1 and false conditional
     * will be executed.
     */
    jmpret = setjmp( mark );
    if( jmpret == 0 )
    {
        /* Try forcing error by dividing by 0 or using very large numbers. */
        printf( "Enter number to divide or CTRL-C: " );
        scanf( "%lf", &n1 );
        printf( "Enter number to divide by or CTRL-C: " );
        scanf( "%lf", &n2 );
        r = n1 / n2;

        /* This won't be reached if error occurs. */
        printf( "\n%4.3g / %4.3g = %4.3g\n", n1, n2, r );
    }
    else
        printf( "Recovered from floating-point error\n" );

    /* Fake CTRL+C. Program will abort or exit, depending on user
     * response in handler.
     */
    raise( SIGINT );
    exit( 0 );
}

/* Function called whenever SIGINT (CTRL+C) interrupt is called. */
void ctrlchandler()
{
    int ch;

    /* Disallow CTRL+C during handler. */
    signal( SIGINT, SIG_IGN );

    printf( "Abort processing? " );
    ch = getche();
    printf( "\n" );
    if( (ch == 'y') || (ch == 'Y') )
        abort();
    else

        /* The CTRL+C interrupt must be reset to our handler since by
         * default DOS resets it to the system handler.
         */
        signal( SIGINT, ctrlchandler );
}

/* Function called whenever SIGABRT interrupt is called. */
void aborthandler()
{
    printf( "The abort() function was called\n" );
    exit( 1 );
}

/* Function called whenever SIGFPE interrupt is called. */
void fphandler( int sig, int num )
{
    printf( "Signal: %d   Subcode: 0x%x\n", sig, num );

    /* Initialize floating-point package. */
    _fpreset();

    /* Restore calling environment and jump back to setjmp. Return -1
     * so that setjmp will return false for conditional test.
     */
    longjmp( mark, -1 );
}
