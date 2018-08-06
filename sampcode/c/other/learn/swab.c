/* SWAB.C illustrates:
 *      swab
 */

#include <stdlib.h>
#include <stdio.h>

char from[] = "BADCFEHGJILKNMPORQTSVUXWZY";
char to[] =   "..........................";

main()
{
    printf( "Before:\t%s\n\t%s\n\n", from, to );
    swab( from, to, sizeof( from ) );
    printf( "After:\t%s\n\t%s\n\n", from, to );
}
