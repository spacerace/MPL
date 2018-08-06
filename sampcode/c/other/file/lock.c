/* LOCK.C illustrates network file sharing functions:
 *      sopen       locking
 *
 * Also the global variable:
 *      _osmajor
 *
 * The program requires DOS 3.0 or higher. The DOS SHARE command must
 * be loaded. The locking mechanism will only work if the program is
 * run from a network drive.
 */

#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <sys\locking.h>
#include <share.h>
#include <stdlib.h>         /* For _osmajor and exit */

void error( char *msg );

main( int argc, char *argv[] )
{
    int handle, i;
    static char msg[] = "Are any of these bytes locked?";
    char buf[1];

    /* Check for DOS version >= 3.0 */
    if( _osmajor < 3 )
        error( "Must be DOS 3.0 or higher" );

    /* If no argument, write file and lock some bytes in it. */
    if( argc == 1 )
    {
        /* Open file with deny none sharing. */
        handle = sopen( "tmpfil", O_BINARY | O_RDWR | O_CREAT,
                                  SH_DENYNO, S_IREAD | S_IWRITE );
        if( handle == -1 )
            error( "Can't open file\n" );

        write( handle, msg, sizeof( msg ) - 1 );

        /* Lock 10 bytes starting at byte 10. */
        lseek( handle, 10L, SEEK_SET );
        if( locking( handle, LK_LOCK, 10L ) )
            error( "Locking failed\n" );
        printf( "Locked 10 bytes starting at byte 10\n" );

        system( "LOCK read" );
        getch();

        /* Unlock. */
        lseek( handle, 10L, SEEK_SET );
        locking( handle, LK_UNLCK, 10L );
        printf( "\nUnlocked 10 bytes starting at byte 10\n" );

        system( "LOCK read" );
        getch();
    }

    /* If argument, Try to read some locked bytes. */
    else
    {
        /* Open file with deny none sharing. */
        handle = sopen( "tmpfil", O_BINARY | O_RDWR,
                                  SH_DENYNO, S_IREAD | S_IWRITE );
        if( handle == -1 )
            error( "Can't open file\n" );

        for( i = 0; i < sizeof( msg ) - 1; i++ )
        {
            /* Print characters until locked bytes are reached. */
            if( read( handle, buf, 1 ) == -1 )
                break;
            else
                putchar( *buf );
        }
    }
    close( handle );
    exit( 0 );
}

void error( char *errmsg )
{
    printf( errmsg );
    exit( 1 );
}
