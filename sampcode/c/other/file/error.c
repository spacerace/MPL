/* ERROR.C illustrates stream file error handling. Functions illustrated
 * include:
 *      ferror          clearerr        exit            _exit
 *      perror          strerror        _strerror
 *
 * The _exit routine is not specifically illustrated, but it is the same
 * as exit except that file buffers are not flushed and atexit and onexit
 * are not called.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
enum BOOL { FALSE, TRUE };

FILE *stream;
char string[] = "This should never be written";
void errortest( FILE *stream, char *msg, int fterm );

main( int argc, char *argv[] )
{
    /* Open file and test to see if open caused error. If so, terminate. */
    stream = fopen( argv[1], "r" );
    errortest( stream, "Can't open file", TRUE );

    /* Try to write to a read-only file, then test to see if write
     * caused error. If so, clear error, but don't terminate.
     */
    fprintf( stream, "%s\n", string );
    errortest( stream, "Can't write file", FALSE );
    exit( 0 );
}

/* Error test routine takes a stream, a message, and a flag telling whether
 * to terminate if there is an error.
 */
void errortest( FILE *stream, char *msg, int fterm )
{
    /* If stream doesn't exist (failed fopen) or if there is an error
     * on the stream, handle error.
     */
    if( (stream == NULL) || (ferror( stream )) )
    {
        perror( msg );
        /* _strerror and strerror can be used to get the same result
         * as perror, as illustrated by these lines (commented out).
        printf( "%s: %s\n", msg, strerror( errno ) );
        printf( _strerror( msg ) );
         */

        /* Terminate or clear error, depending on terminate flag. */
        if( fterm )
            exit( errno );
        else
            clearerr( stream );
    }
}
