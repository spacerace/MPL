/* TYPEIT.C illustrates reassigning handles and streams using functions:
 *      freopen         dup             dup2
 *
 * The example also illustrates:
 *      setargv
 *
 * To make the program handle wild cards, link it with the SETARGV.OBJ
 * file. You can do this in the QC environment by creating a program list
 * containg TYPEIT.C and SETARGV.OBJ (include the path or put in current
 * directory). You must also turn off the Extended Dictionary flag
 * within the QC environment (Options-Make-Linker Flags) or use the /NOE
 * linker option outside the environment. For example:
 *    QCL typeit.c setargv /link /NOE
 */

#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <process.h>

main( int argc, char **argv )
{
    FILE *ftmp;
    int htmp;

    /* Duplicate handle of stdin. Save the original to restore later. */
    htmp = dup( fileno( stdin ) );

    /* Process each command line argument. */
    while( *(++argv) != NULL )
    {
        /* Original stdin used for getch. */
        printf( "Press any key to display file: %s\n", *argv );
        getch();

        /* Reassign stdin to input file. */
        ftmp = freopen( *argv, "rb", stdin );

        if( (ftmp == NULL) || (htmp == -1 ) )
        {
            perror( "Can't reassign standard input" );
            exit( 1 );
        }

        /* Spawn MORE, which will receive the open input file as its standard
         * input. MORE can be the DOS MORE or the sample program MORE.C.
         */
        spawnvp( P_WAIT, "MORE", NULL );

        /* Reassign stdin back to original so that we can use the
         * original stdin to get a key.
         */
        dup2( htmp, fileno( stdin ) );
    }
    exit( 0 );
}
