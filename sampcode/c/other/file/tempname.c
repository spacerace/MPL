/* TEMPNAME.C illustrates:
 *      tmpnam          tempnam
 */

#include <stdio.h>

main()
{
    char *name1, *name2;
    int c;

    /* Create several temporary file names in the current directory. */
    for( c = 0; c < 5; c++ )
        if( (name1 = tmpnam( NULL )) != NULL )
            printf( "%s is a safe temporary file name.\n", name1 );

    /* Create a temporary file name with prefix TEMP and place it in
     * the first of these directories that exists:
     *   1. TMP environment directory
     *   2. C:\TEMPFILE
     *   3. P_tmpdir directory (defined in stdio.h)
     */
    if( (name2 = tempnam( "C:\\TEMPFILE", "TEMP" )) != NULL )
        printf( "%s is a safe temporary file name.\n", name2 );

}
