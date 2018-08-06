/* ASSERT.C illustrates function:
 *      assert
 */

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <assert.h>

#define MAXSTR 120

void chkstr( char *string );    /* Prototype */

main()
{
    char string1[MAXSTR], string2[MAXSTR];

    /* Do various processes on strings and check the results. If
     * none cause errors, force on error with empty string.
     */
    printf( "Enter a string: " );
    gets( string1 );
    chkstr( string1 );

    printf( "Enter another string: " );
    gets( string2 );
    chkstr( string2 );

    strcat( string1, string2 );
    chkstr( string1 );
    printf( "string1 + string2 = %s\n", string1 );

    chkstr( "" );
    printf( "You'll never get here\n" );
}

void chkstr( char *string )
{
    assert( string != NULL );               /* Cannot be NULL */
    assert( *string != '\0' );              /* Cannot be empty */
    assert( strlen( string ) < MAXSTR );    /* Length must be positive */
}
