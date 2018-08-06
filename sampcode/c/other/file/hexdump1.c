/* HEXDUMP.C illustrates directory splitting and character stream I/O.
 * Functions illustrated include:
 *      _splitpath      _makepath
 *      fgetc           fputc           fgetchar        fputchar
 *      getc            putc            getchar         putchar
 *
 * While fgetchar, getchar, fputchar, and putchar are not specifically
 * used in the example, they are equivalent to using fgetc or getc with
 * stdin, or to using fputc or putc with stdout. See FUNGET.C for another
 * example of fgetc and getc.
 */

#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>

main()
{
    FILE *infile, *outfile;
    char inpath[_MAX_PATH], outpath[_MAX_PATH];
    char drive[_MAX_DRIVE], dir[_MAX_DIR];
    char fname[_MAX_FNAME], ext[_MAX_EXT];
    int  in, size;
    long i = 0L;

    /* Query for and open input file. */
    printf( "Enter input file name: " );
    gets( inpath );
    strcpy( outpath, inpath );
    if( (infile = fopen( inpath, "rb" )) == NULL )
    {
        printf( "Can't open input file" );
        exit( 1 );
    }

    /* Build output file by splitting path and rebuilding with
     * new extension.
     */
    _splitpath( outpath, drive, dir, fname, ext );
    strcpy( ext, "hx" );
    _makepath( outpath, drive, dir, fname, ext );

    /* If file does not exist, open it */
    if( access( outpath, 0 ) )
    {
        outfile = fopen( outpath, "wb" );
        printf( "Creating %s from %s . . .\n", outpath, inpath );
    }
    else
    {
        printf( "Output file already exists" );
        exit( 1 );
    }

    printf( "(B)yte or (W)ord: " );
    size = getche();

    /* Get each character from input and write to output. */
    while( !feof( infile ) )
    {
        if( (size == 'W') || (size == 'w') )
        {
            in = getw( infile );
            fprintf( outfile, " %.4X", in );
            if( !(++i % 8) )
                putw( 0x0D0A, outfile );        /* New line      */
        }
        else
        {
            /* This example uses the fgetc and fputc functions. You
             * could also use the macro versions:
            in = getc( infile );
             */
            in = fgetc( infile );
            fprintf( outfile, " %.2X", in );
            if( !(++i % 16) )
            {
                /* Macro version:
                putc( 13, outfile );
                 */
                fputc( 13, outfile );           /* New line      */
                fputc( 10, outfile );
            }
        }
    }
    fclose( infile );
    fclose( outfile );
    exit( 0 );
}
