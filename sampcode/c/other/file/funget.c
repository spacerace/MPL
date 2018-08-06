/* FUNGET.C illustrates getting and ungetting characters from a file.
 * Functions illustrated include:
 *      getc            getchar         ungetc
 *      fgetc           fgetchar
 *
 * Although getchar and fgetchar are not specifically used in the example,
 * they are equivalent to using getc or fgetc with stdin. See HEXDUMP.C
 * for another example of getc and fgetc.
 */

#include <conio.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

void getword( FILE *stream, char *buf );
void skiptoword( FILE *stream );

main()
{
    char buffer[128];
    FILE *infile;

    printf( "Enter file name: " );
    gets( buffer );
    if( (infile = fopen( buffer, "rb" )) == NULL )
    {
        perror( "Can't open file" );
        exit( 1 );
    }

    /* Read each word and print reversed version. */
    while( 1 )
    {
        skiptoword( infile );
        getword( infile, buffer );
        puts( strrev( buffer ) );
    }
}

/* Read one word (defined as a string of alphanumeric characters). */
void getword( FILE *stream, char *p )
{
    int  ch;

    do
    {
        /* Macro version used here, but function version could be used:
        ch = fgetc( stream );
         */
        ch = getc( stream );        /* Get characters until EOF  */
        if( ch == EOF )             /*   or non-alphanumeric     */
            exit( 0 );
        *(p++) = (char)ch;
    } while( isalnum( ch ) );
    ungetc( ch, stream );           /* Put non-alphanumeric back */
    *(--p) = '\0';                  /* Null-terminate            */
}

/* Throw away non-digit characters. */
void skiptoword( FILE *stream )
{
    int  ch;

    do
    {
        ch = getc( stream );
        if( ch == EOF )
            exit( 0 );
    } while( !isalnum( ch ) );
    ungetc( ch, stream );
}
