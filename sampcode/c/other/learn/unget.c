/* UNGET.C illustrates getting and ungetting characters from the console.
 * Functions illustrated include:
 *      getch           ungetch         putch
 */

#include <conio.h>
#include <stdio.h>
#include <ctype.h>

void skiptodigit( void );
int getnum( void );

main()
{
    int c, array[10];

    /* Get five numbers from console. Then display them. */
    for( c = 0; c < 5; c++ )
    {
        skiptodigit();
        array[c] = getnum();
        printf( "\t" );
    }
    for( c  = 0; c < 5; c++ )
        printf( "\n\r%d", array[c] );
    printf( "\n" );
}

/* Convert digit characters into a number until a non-digit is received. */
int getnum()
{
    int ch, num =  0;

    while( isdigit( ch = getch() ) )
    {
        putch( ch );                        /* Display digit      */
        num = (num * 10) + ch  - '0';       /* Convert to number  */
    }
    ungetch( ch );                          /* Put non-digit back */
    return num;                             /* Return result      */
}

/* Throw away non-digit characters. */
void skiptodigit()
{
    int ch;

    while( !isdigit( ch = getch() ) )
        ;
    ungetch( ch );
}
