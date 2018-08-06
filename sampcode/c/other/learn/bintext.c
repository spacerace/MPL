/* BINTEXT.C illustrates changing between binary and text modes for
 * stream I/O using function:
 *      setmode
 * and global variable:
 *      _fmode
 */

#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <string.h>
#include <stdlib.h>         /* For _fmode and exit */

main()
{
    FILE *htmp;
    char name[13];

    /* Set default mode to binary and open file. */
    _fmode = O_BINARY;
    if( (htmp = fopen( tmpnam( name ), "w+" )) == NULL )
        exit( 1 );
    fprintf( htmp, "\nThis\nis\nsome\nbinary\ntext.\n\n" );

    /* Flush buffer and change mode to text. */
    fflush( htmp );
    setmode( fileno( htmp ), O_TEXT );
    fprintf( htmp, "\nThis\nis\nsome\ntext\ntext.\n\n" );
    fclose( htmp );

    system( strcat( "TYPE ", name ) );
    remove( name );
    exit( 0 );
}
