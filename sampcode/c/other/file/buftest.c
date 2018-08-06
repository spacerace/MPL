/* BUFTEST.C illustrates buffer control for stream I/O using functions:
 *      setbuf              setvbuf
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define BUF2SIZ 2048

long countln( FILE *stream );           /* Prototype        */
char buf1[BUFSIZ], buf2[BUF2SIZ];       /* File buffers     */

main( int argc, char *argv[] )
{
    time_t start;
    FILE *stream;
    int  c;

    /* Use our buffer with the default size. This gives us the option
     * of examining and/or modifying the buffer during I/0 (though the
     * example doesn't illustrate this).
     */
    if( (stream = fopen( argv[1], "rt" )) == NULL )
        exit( 1 );
    setbuf( stream, buf1 );
    start = clock();
    c = countln( stream );
    printf( "Time: %5.2f\tBuffering: Normal\tBuffer size: %d\n",
             ((float)clock() - start) / CLK_TCK, BUFSIZ );

    /* Use a larger buffer. */
    if( (stream = fopen( argv[1], "rt" )) == NULL )
        exit( 1 );
    setvbuf( stream, buf2, _IOFBF, sizeof( buf2 ) );
    start = clock();
    c = countln( stream );
    printf( "Time: %5.2f\tBuffering: Normal\tBuffer size: %d\n",
             ((float)clock() - start) / CLK_TCK, BUF2SIZ );

    /* Try it with no buffering. */
    if( (stream = fopen( argv[1], "rt" )) == NULL )
        exit( 1 );
    setvbuf( stream, NULL, _IONBF, 0 );
    start = clock();
    c = countln( stream );
    printf( "Time: %5.2f\tBuffering: Normal\tBuffer size: %d\n",
             ((float)clock() - start) / CLK_TCK, 0 );

    printf( "File %s has %d lines", argv[1], c );
    exit( 0 );
}

/* Count lines in text files and close file. */
long countln( FILE *stream )
{
    char linebuf[120];
    long c = 0;

    while( !feof( stream ) )
    {
        fgets( linebuf, 121, stream );
        ++c;
    }
    fclose( stream );
    return c;
}
