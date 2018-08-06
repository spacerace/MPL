/* RECORDS1.C illustrates reading and writing of file records using seek
 * functions including:
 *      fseek       rewind      ftell
 *
 * Other general functions illustrated include:
 *      tmpfile     rmtmp       fread       fwrite
 *
 * Also illustrated:
 *      struct
 *
 * See RECORDS2.C for a version program using fgetpos and fsetpos.
 */

#include <stdio.h>
#include <io.h>
#include <string.h>

/* File record */
struct RECORD
{
    int     integer;
    long    doubleword;
    double  realnum;
    char    string[15];
} filerec = { 0, 1, 10000000.0, "eel sees tar" };

main()
{
    int c, newrec;
    size_t recsize = sizeof( filerec );
    FILE *recstream;
    long recseek;

    /* Create and open temporary file. */
    recstream = tmpfile();

    /* Write 10 unique records to file. */
    for( c = 0; c < 10; c++ )
    {
        ++filerec.integer;
        filerec.doubleword *= 3;
        filerec.realnum /= (c + 1);
        strrev( filerec.string );

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
            recseek = (long)((newrec - 1) * recsize);
            fseek( recstream, recseek, SEEK_SET );

            fread( &filerec, recsize, 1, recstream );

            printf( "Integer:\t%d\n", filerec.integer );
            printf( "Doubleword:\t%ld\n", filerec.doubleword );
            printf( "Real number:\t%.2f\n", filerec.realnum );
            printf( "String:\t\t%s\n\n", filerec.string );
        }
    } while( newrec );

    /* Starting at first record, scan each for specific value. The following
     * line is equivalent to:
     *      fseek( recstream, 0L, SEEK_SET );
     */
    rewind( recstream );

    do
    {
        fread( &filerec, recsize, 1, recstream );
    } while( filerec.doubleword < 1000L );

    recseek = ftell( recstream );
    /* Equivalent to: recseek = fseek( recstream, 0L, SEEK_CUR ); */
    printf( "\nFirst doubleword above 1000 is %ld in record %d\n",
            filerec.doubleword, recseek / recsize );

    /* Close and delete temporary file. */
    rmtmp();
}
