/* CMPSTR.C illustrates string and memory comparison functions including:
 *       memcmp        memicmp
 *       strncmp       strnicmp
 *       strcmp        stricmp          strcmpi
 */

#include <memory.h>
#include <string.h>
#include <stdio.h>

char string1[] = "The quick brown dog jumps over the lazy fox";
char string2[] = "The QUICK brown fox jumps over the lazy dog";

main()
{
    char tmp[20];
    int result;

    printf( "Compare strings:\n\t\t%s\n\t\t%s\n\n", string1, string2 );

    printf( "Function:\tmemcmp\n" );
    result = memcmp( string1, string2 , 42 );
    if( result > 0 )
        strcpy( tmp, "greater than" );
    else if( result < 0 )
        strcpy( tmp, "less than" );
    else
        strcpy( tmp, "equal to" );
    printf( "Result:\t\tString 1 is %s than string 2\n\n", tmp );

    printf( "Function:\tmemicmp\n" );
    result = memicmp( string1, string2, 42 );
    if( result > 0 )
        strcpy( tmp, "greater than" );
    else if( result < 0 )
        strcpy( tmp, "less than" );
    else
        strcpy( tmp, "equal to" );
    printf( "Result:\t\tString 1 is %s than string 2\n\n", tmp );

    printf( "Function:\tstrncmp\n" );
    result = strncmp( string1, string2 , 42 );
    if( result > 0 )
        strcpy( tmp, "greater than" );
    else if( result < 0 )
        strcpy( tmp, "less than" );
    else
        strcpy( tmp, "equal to" );
    printf( "Result:\t\tString 1 is %s than string 2\n\n", tmp );

    printf( "Function:\tstrnicmp\n" );
    result = strnicmp( string1, string2, 42 );
    if( result > 0 )
        strcpy( tmp, "greater than" );
    else if( result < 0 )
        strcpy( tmp, "less than" );
    else
        strcpy( tmp, "equal to" );
    printf( "Result:\t\tString 1 is %s than string 2\n\n", tmp );

    printf( "Function:\tstrcmp\n" );
    result = strcmp( string1, string2 );
    if( result > 0 )
        strcpy( tmp, "greater than" );
    else if( result < 0 )
        strcpy( tmp, "less than" );
    else
        strcpy( tmp, "equal to" );
    printf( "Result:\t\tString 1 is %s than string 2\n\n", tmp );

    printf( "Function:\tstricmp or strcmpi\n" );
    result = stricmp( string1, string2 );
    /* strcmpi (commented out) is the same as stricmp.
    result = strcmpi( string1, string2 );
     */
    if( result > 0 )
        strcpy( tmp, "greater than" );
    else if( result < 0 )
        strcpy( tmp, "less than" );
    else
        strcpy( tmp, "equal to" );
    printf( "Result:\t\tString 1 is %s than string 2\n", tmp );
}
