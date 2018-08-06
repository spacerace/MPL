/* TOKEN.C illustrates tokenizing and searching for any of several
 * characters. Functions illustrated include:
 *      strcspn         strspn          strpbrk         strtok
 */

#include <string.h>
#include <stdio.h>

main()
{
    char string[100];
    static char vowels[] = "aeiouAEIOU", seps[] = " \t\n,";
    char *p;
    int  count;

    printf( "Enter a string: " );
    gets( string );

    /* Delete one word at a time. */
    p = string;
    while( *p )
    {
        printf( "String remaining: %s\n", p );
        p += strcspn( p, seps );    /* Find next separator     */
        p += strspn( p, seps );     /* Find next non-separator */
    }

    /* Count vowels. */
    p = string;
    count = 0;
    while( *(p - 1) )
    {
        p = strpbrk( p, vowels );   /* Find next vowel         */
        p++;
        count++;
    }
    printf( "\nVowels in string: %d\n\n", count - 1 );

    /* Break into tokens. */
    count = 0;
    p = strtok( string, seps );     /* Find first token        */
    while( p != NULL )
    {
        printf( "Token %d: %s\n", ++count, p );
        p = strtok( NULL, seps );   /* Find next token         */
    }
}
