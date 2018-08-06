/* RECORDS2.C illustrates reading and writing of file records with the
 * following functions:
 *      fgetpos     fsetpos
 *
 * See RECORDS1.C for a version using fseek, rewind, and ftell.
 */

#include <stdio.h>
#include <io.h>

/* File record */
struct RECORD
{
    int     integer;
    long    doubleword;
    double  realnum;
} filerec = { 0, 1, 10000000.0 };

main()
{
    int c, newrec;
    size_t recsize = sizeof( filerec );
    FILE *recstream;
    fpos_t *recpos;

    /* Create and open temporary file. */
    recstream = tmpfile();

    /* Write 10 unique records to file. */
    for( c = 0; c < 10; c++ )
    {
        ++filerec.integer;
        filerec.doubleword *= 3;
        filerec.realnum /= (c + 1);

        fwrite( &filerec, recsize, 1, recstream );
    }

    /* Find a specified record. */
    do
    {
        printf( "Enter record betweeen 1 and 10 (or 0 to quit): " );
        scanf( "%d", &newrec );

        /* Find and display valid records. */
        if( (newrec >= 1) && (newrec <= 10) )
        {
            *recpos = (fpos_t)((newrec - 1) * recsize);
            fsetpos( recstream, recpos );
            fread( &filerec, recsize, 1, recstream );

            printf( "Integer:\t%d\n", filerec.integer );
            printf( "Doubleword:\t%ld\n", filerec.doubleword );
            printf( "Real number:\t%.2f\n", filerec.realnum );
        }
    } while( newrec );

    /* Starting at first record, scan each for specific value. */
    *recpos = (fpos_t)0;
    fsetpos( recstream, recpos );
    do
    {
        fread( &filerec, recsize, 1, recstream );
    } while( filerec.doubleword < 1000L );

    fgetpos( recstream, recpos );
    printf( "\nFirst doubleword above 1000 is %ld in record %d\n",
            filerec.doubleword, *recpos / recsize );

    /* Close and delete temporary file. */
    rmtmp();
}
