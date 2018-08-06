/* PAGER.C illustrates line input and output on stream text files.
 * Functions illustrated include:
 *      fopen           fclose          fcloseall       feof
 *      fgets           fputs           rename
 */

#include <stdio.h>
#include <io.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>

#define MAXSTRING 128
#define PAGESIZE 55
enum BOOL { FALSE, TRUE };

void quit( int error );
FILE *infile, *outfile;
char outpath[] = "tmXXXXXX";

main( int argc, char *argv[] )
{
    char tmp[MAXSTRING];
    long line = 0, page = 1;
    int  endflag = FALSE;

    /* Open file (from command line) and output (temporary) files. */
    if( (infile = fopen( argv[1], "rt" )) == NULL )
        quit( 1 );
    mktemp( outpath );
    if( (outfile = fopen( outpath, "wt" )) == NULL )
        quit( 2 );

    /* Get each line from input and write to output. */
    while( 1 )
    {
        /* Insert form feed and page header at the top of each page. */
        if( !(line++ % PAGESIZE) )
            fprintf( outfile, "\f\nPage %d\n\n", page++ );

        if( fgets( tmp, MAXSTRING - 1, infile ) == NULL )
            if( feof( infile ) )
                break;
            else
                quit( 3 );
        if( fputs( tmp, outfile ) == EOF )
            quit( 3 );
    }

    /* Close files and move temporary file to original by deleting
     * original and renaming temporary.
     */
    fcloseall();
    remove( argv[1] );
    rename( outpath, argv[1] );
    exit( 0 );
}

/* Handle errors. */
void quit( int error )
{
    switch( error )
    {
        case 1:
            perror( "Can't open input file" );
            break;
        case 2:
            perror( "Can't open output file" );
            fclose( infile );
            break;
        case 3:
            perror( "Error processing file" );
            fclose( infile );
            fclose( outfile );
            remove( outpath );
            break;
    }
    exit( error );
}
