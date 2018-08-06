/* CASE.C illustrates case conversion and other conversions.
 * Functions illustrated include:
 *      strupr          toupper         _toupper
 *      strlwr          tolower         _tolower
 *      strrev          toascii
 */

#include <string.h>
#include <stdio.h>
#include <ctype.h>

char mstring[] = "Dog Saw Dad Live On";
char *ustring, *tstring, *estring;
char *p;

main()
{
    printf( "Original:\t%s\n", mstring );

    /* Upper and lower case. */
    ustring = strupr( strdup( mstring ) );
    printf( "Upper case:\t%s\n", ustring );

    printf( "Lower case:\t%s\n", strlwr( ustring ) );

    /* Reverse case of each character. */
    tstring = strdup( mstring );
    for( p = tstring; *p; p++ )
    {
        if( isupper( *p ) )
            *p = tolower( *p );
        else
            *p = toupper( *p );

        /* This alternate code (commented out) shows how to use _tolower
         * and _toupper for the same purpose.
        if( isupper( *p ) )
            *p = _tolower( *p );
        else if( islower( *p ) )
            *p = _toupper( *p );
         */
    }
    printf( "Toggle case:\t%s\n", tstring );

    /* Encode and decode string. The decoding technique will convert
     * WordStar-style strings, which have some high bits set.
     */
    estring = strdup( mstring );
    for( p = estring; *p; p++ )
        *p = *p | 0x80;
    printf( "Encoded:\t%s\n", estring );

    for( p = estring; *p; p++ )
        *p = toascii( *p );
    printf( "Decoded:\t%s\n", estring );

    printf( "Reversed:\t%s\n", strrev( ustring ) );
}
