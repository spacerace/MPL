/* SETSTR.C illustrate string and memory set functions including:
 *     memset           strnset         strset
 */

#include <memory.h>
#include <string.h>
#include <stdio.h>

char string[60] = "The quick brown dog jumps over the lazy fox ";
/*                          1         2         3         4         5 *
 *                 12345678901234567890123456789012345678901234567890 */
main()
{
    printf( "Function:\tmemset with fill character 'Û'\n" );
    printf( "Destination:\t%s\n", string );
    memset( string + 10, 'Û', 5 );
    printf( "Result:\t\t%s\n\n", string );

    printf( "Function:\tstrnset with fill character 'Û'\n" );
    printf( "Destination:\t%s\n", string );
    strnset( string + 15, 'Û', 5 );
    printf( "Result:\t\t%s\n\n", string );

    printf( "Function:\tstrset with fill character 'Û'\n" );
    printf( "Destination:\t%s\n", string );
    strset( string + 20, 'Û' );
    printf( "Result:\t\t%s\n\n", string );
}
