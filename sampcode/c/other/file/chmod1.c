/* CHMOD1.C illustrates reading and changing file attribute and time using:
 *      access          chmod           utime
 *
 * See CHMOD2.C for a more powerful variation of this program using
 * _dos functions.
 */

#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <sys\utime.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

enum FILEATTRIB { EXIST, WRITE = 2, READ = 4, READWRITE = 6 };

/* Exist macro uses access */
#define EXIST( name ) !access( name, EXIST )

main( int argc, char *argv[] )
{
    if( !EXIST( argv[1] ) )
    {
        printf( "Syntax:  CHMOD1 <filename>" );
        exit( 1 );
    }

    if( !access( argv[1], WRITE ) )
    {
        printf( "File %s is read/write. Change to read only? ", argv[1] );

        /* Note: Use stdlib.h for function definition of toupper rather
         * than macro version in ctype.h. Macro side effects would cause
         * macro version to read two keys.
         */
        if( toupper( getch() ) == 'Y' )
            chmod( argv[1], S_IREAD );
    }
    else
    {
        printf( "File %s is read only. Change to read/write? ", argv[1] );
        if( toupper( getch() ) == 'Y' )
            chmod( argv[1], S_IREAD | S_IWRITE );
    }

    printf( "\nUpdate file time to current time? " );
    if( toupper( getch() ) == 'Y' )
        utime( argv[1], NULL );
    exit( 0 );
}
