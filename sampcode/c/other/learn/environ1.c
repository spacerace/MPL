/* ENVIRON.C illustrates environment variable functions including:
 *      getenv          putenv          _searchenv
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

main()
{
    char *pathvar, pathbuf[128], filebuf[128];

    /* Get the PATH environment variable and save a copy of it. */
    pathvar = getenv( "PATH" );
    strcpy( pathbuf, pathvar );
    printf( "Old PATH: %s\n", pathvar ? pathvar : "variable not set");

    /* Add a new directory to the path. */
    strcat( pathbuf, ";\\QC;" );
    if( putenv( pathbuf ) == -1 )
    {
        printf( "Failed\n");
        return 1;
    }
    else
        printf( "New PATH: %s\n", pathbuf );

    /* Search for file in the new path. */
    _searchenv( "QC.INI", "PATH", filebuf );
    if( *filebuf )
        printf( "QC.INI found at %s\n", filebuf );
    else
        printf( "QC.INI not found\n" );

    /* Restore original path. */
    if( putenv( pathvar ) == -1 )
        printf( "Failed\n");
    else
        printf( "Old PATH: %s\n", pathvar );
    return 0;
}
