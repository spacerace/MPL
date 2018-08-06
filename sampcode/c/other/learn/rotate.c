/* ROTATE.C illustrates bit rotation functions including:
 *      _rotl           _rotr           _lrotl          _lrotr
 *
 * _lrotl and _lrotr are not shown in the program, but they are the
 * same as _rotl and _rotr except that they work on long integers.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *binstr( int num, char *buffer );      /* Prototype */

main()
{
    int  shift, i, ir, il;
    char tmpbuf[20];

    printf( "Enter integer: " );
    scanf( "%d", &i );
    printf( "\n\n" );

    /* Display table header for rotates. */
    printf( "%6s    %-7s    %16s    %-7s    %16s\n",
            " ", "Left", " ", "Right", " " );
    printf( "%6s    %7s    %16s    %7s    %16s\n\n",
            "Shift", "Decimal", "Binary", "Decimal", "Binary" );

    /* Display table of rotated values. */
    for( shift = 0; shift <= 16; shift++ )
    {
        il = _rotl( i, shift ); ;
        printf( "%5d    %7d    %16s    ", shift, il, binstr( il,  tmpbuf ) );
        ir = _rotr( i, shift );
        printf( "%7d    %16s\n", ir, binstr( ir, tmpbuf ) );
    }
}

/* Convert integer to string of 16 binary characters. */
char *binstr( int num, char *buffer )
{
    char tmp[17];
    int  len;

    memset( buffer, '0', 16 );
    len = strlen( itoa( num, tmp, 2 ) );
    strcpy( buffer + 16 - len, tmp );
    return buffer;
}
