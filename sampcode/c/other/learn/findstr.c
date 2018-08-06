/* FINDSTR.C illustrates memory and string search functions including:
 *       memchr        strchr        strrchr            strstr
 */

#include <memory.h>
#include <string.h>
#include <stdio.h>

int  ch = 'r';
char str[] =    "lazy";
char string[] = "The quick brown dog jumps over the lazy fox";
char fmt1[] =   "         1         2         3         4         5";
char fmt2[] =   "12345678901234567890123456789012345678901234567890";

main()
{
    char *pdest;
    int result;

    printf( "String to be searched:\n\t\t%s\n", string );
    printf( "\t\t%s\n\t\t%s\n\n", fmt1, fmt2 );

    printf( "Function:\tmemchr\n" );
    printf( "Search char:\t%c\n", ch );
    pdest = memchr( string, ch, sizeof( string ) );
    result = pdest - string + 1;
    if( pdest != NULL )
        printf( "Result:\t\t%c found at position %d\n\n", ch, result );
    else
        printf( "Result:\t\t%c not found\n" );

    printf( "Function:\tstrchr\n" );
    printf( "Search char:\t%c\n", ch );
    pdest = strchr( string, ch );
    result = pdest - string + 1;
    if( pdest != NULL )
        printf( "Result:\t\t%c found at position %d\n\n", ch, result );
    else
        printf( "Result:\t\t%c not found\n" );

    printf( "Function:\tstrrchr\n" );
    printf( "Search char:\t%c\n", ch );
    pdest = strrchr( string, ch );
    result = pdest - string + 1;
    if( pdest != NULL )
        printf( "Result:\t\t%c found at position %d\n\n", ch, result );
    else
        printf( "Result:\t\t%c not found\n" );

    printf( "Function:\tstrstr\n" );
    printf( "Search string:\t%s\n", str );
    pdest = strstr( string, str );
    result = pdest - string + 1;
    if( pdest != NULL )
        printf( "Result:\t\t%s found at position %d\n\n", str, result );
    else
        printf( "Result:\t\t%c not found\n" );
}
