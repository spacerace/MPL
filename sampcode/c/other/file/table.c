/* TABLE.C illustrates reading and writing formatted file data using
 * functions:
 *      fprintf         fscanf
 */

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <stdlib.h>

main()
{
    char buf[128];
    FILE *ftable;
    long l, tl;
    float fp, tfp;
    int  i, c = 'A';

    /* Open an existing file for reading. Fail if file doesn't exist. */
    if( (ftable = fopen( "table.smp", "r")) != NULL )
    {
        /* Read data from file and total it. */
        for( i = 0, tl = 0, tfp = 0.0; i < 10; i++ )
        {
            fscanf( ftable, "\t%s %c: %ld %f\n", buf, &c, &l, &fp );
            tl += l;
            tfp += fp;
            printf( "\t%s %c: %7ld %9.2f\n", buf, c, l, fp );
        }
        printf( "\n\tTotal:  %7ld %9.2f\n", tl, tfp );
    }
    else
    {
        /* File did not exist. Create it for writing. */
        if( (ftable = fopen( "table.smp", "w" )) == NULL )
            exit( 1 );

        /* Write table to file. */
        for( i = 0, l = 99999, fp = 3.14; i < 10; i++ )
            fprintf( ftable, "\tLine %c: %7ld %9.2f\n",
                     c++, l /= 2, fp *= 2 );

        printf( "Created table file. Run again to read it.\n" );
    }
    fclose( ftable );
    exit( 0 );
}
