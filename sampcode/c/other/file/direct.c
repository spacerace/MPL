/* DIRECT.C illustrates directory functions and miscellaneous file
 * and other functions including:
 *      getcwd          mkdir           chdir           rmdir
 *      system          mktemp          remove          unlink
 *      stat
 *
 * See NULLFILE.C for an example of fstat, which is similar to stat.
 */

#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

main()
{
    char cwd[_MAX_DIR];
    static char tmpdir[] = "DRXXXXXX";
    struct stat filestat;

    /* Get the current working directory. */
    getcwd( cwd, _MAX_DIR );

    /* Try to make temporary name for directory. */
    if( mktemp( tmpdir ) == NULL )
    {
        perror( "Can't make temporary directory" );
        exit( 1 );
    }

    /* Try to create a new directory, and if successful, change to it. */
    if( !mkdir( tmpdir ) )
    {
        chdir( tmpdir );

        /* Create and display a file to prove it. */
        system( "echo This is a test. > TEST.TXT" );
        system( "type test.txt" );

        /* Display some file statistics. */
        if( !stat( "TEST.TXT", &filestat ) )
        {
            printf( "File: TEST.TXT\n" );
            printf( "Drive %c:\n", filestat.st_dev + 'A' );
            printf( "Directory: %s\\%s\n", cwd + 2, tmpdir );
            printf( "Size: %ld\n", filestat.st_size );
            printf( "Created: %s", ctime( &filestat.st_atime ) );
        }
        getch();

        /* Delete file, go back to original directory, and remove
         * directory. Note that under DOS, remove is equivalent to unlink,
         * so this line could be used instead.
        unlink( "TEST.TXT" );
         */
        remove( "TEST.TXT" );
        chdir( cwd );
        rmdir( tmpdir );
    }
}
