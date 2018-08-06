/* SEEK.C illustrates low-level file I/O functions including:
 *      filelength      lseek           tell
 */

#include <io.h>
#include <conio.h>
#include <stdio.h>
#include <fcntl.h>          /* O_ constant definitions */
#include <process.h>

void error( char *errmsg );

main()
{
    int handle, ch;
    unsigned count;
    long position, length;
    char buffer[2], fname[80];

    /* Get file name and open file. */
    do
    {
        printf( "Enter file name: " );
        gets( fname );
        handle = open( fname, O_BINARY | O_RDONLY );
    } while( handle == -1 );

    /* Get and print length. */
    length = filelength( handle );
    printf( "\nFile length of %s is: %ld\n\n", fname, length );

    /* Report the character at a specified position. */
    do
    {
        printf( "Enter integer position less than file length: " );
        scanf( "%ld", &position );
    } while( position > length );

    lseek( handle, position, SEEK_SET );
    if( read( handle, buffer, 1 ) == -1 )
        error( "Read error" );
    printf( "Character at byte %ld is ASCII %u ('%c')\n\n",
            position, *buffer, *buffer );

    /* Search for a specified character and report its position. */
    lseek( handle, 0L, SEEK_SET);           /* Set to position 0 */
    printf( "Type character to search for: " );
    ch = getche();

    /* Read until character is found */
    do
    {
        if( (count = read( handle, buffer, 1 )) == -1 )
            error( "Read error" );
    } while( (*buffer != (char)ch) && count );

    /* Report the current position. */
    position = tell( handle );
    if( count )
        printf( "\nFirst ASCII %u ('%c') is at byte %ld\n",
                ch, ch, position );
    else
        printf( "\nASCII %u ('%c') not found\n", ch, ch );
    close( handle );
    exit( 0 );
}

void error( char *errmsg )
{
    perror( errmsg );
    exit( 1 );
}
