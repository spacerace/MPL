/* GETCH.C illustrates how to process ASCII or extended keys.
 * Functions illustrated include:
 *      getch           getche
 */

#include <conio.h>
#include <ctype.h>
#include <stdio.h>

main()
{
    int key;

    /* Read and display keys until ESC is pressed. */
    while( 1 )
    {
        /* If first key is 0, then get second extended. */
        if( !(key = getch()) )
        {
            key = getch();
            printf( "ASCII: no\tChar: NA\t" );
        }

        /* Otherwise there's only one key. */
        else
            printf( "ASCII: yes\tChar: %c \t", isgraph( key ) ? key : ' ' );

        printf( "Decimal: %d\tHex: %X\n", key, key );

        /* Echo character response to prompt. */
        if( key == 27)
        {
            printf( "Do you really want to quit? (Y/n) " );
            key = getche();
            printf( "\n" );
            if( (toupper( key ) == 'Y') || (key == 13) )
                break;
        }
    }
}
