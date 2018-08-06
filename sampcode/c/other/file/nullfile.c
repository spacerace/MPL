/* NULLFILE.C illustrates these functions:
 *      chsize          umask           setmode
 *      creat           fstat
 */

#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <stdlib.h>
#include <time.h>

main()
{
    int fhandle;
    long fsize;
    struct stat fstatus;
    char fname[80];

    /* Create a file of a specified length. */
    printf( "What dummy file do you want to create: " );
    gets( fname );
    if( !access( fname, 0 ) )
    {
        printf( "File exists" );
        exit( 1 );
    }

    /* Mask out write permission. This means that a later call to open
     * will not be able to set write permission. This is not particularly
     * useful in DOS, but umask is provided primarily for compatibility
     * with systems (such as Unix) that allow multiple permission levels.
     */
    umask( S_IWRITE );

    /* Despite write request, file is read only because of mask. */
    if( (fhandle = creat( fname, S_IREAD | S_IWRITE )) == -1 )
    {
        printf( "File can't be created" );
        exit( 1 );
    }

    /* Since creat uses the default mode (usually text), you must
     * use setmode to make sure the mode is binary.
     */
    setmode( fhandle, O_BINARY );

    printf( "How long do you want the file to be? " );
    scanf( "%ld", &fsize );
    chsize( fhandle, fsize );

    /* Display statistics. */
    fstat( fhandle, &fstatus );
    printf( "File: %s\n", fname );
    printf( "Size: %ld\n", fstatus.st_size );
    printf( "Drive %c:\n", fstatus.st_dev + 'A' );
    printf( "Permission: %s\n",
            (fstatus.st_mode & S_IWRITE) ? "Read/Write" : "Read Only" );
    printf( "Created: %s", ctime( &fstatus.st_atime ) );

    close( fhandle );
    exit( 0 );
}
