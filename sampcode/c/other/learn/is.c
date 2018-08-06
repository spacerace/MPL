/* IS.C illustrates character classification functions including:
 *      isprint         isascii         isalpha         isalnum
 *      isupper         islower         isdigit         isxdigit
 *      ispunct         isspace         iscntrl         isgraph
 *
 * Console output is also shown using:
 *      cprintf
 *
 * See PRINTF.C for additional examples of formatting for cprintf.
 */

#include <ctype.h>
#include <conio.h>

main()
{
    int ch;

    /* Display each ASCII character with character type table. */
    for( ch = 0; ch < 256; ch++ )
    {
        if( ch % 22 == 0 )
        {
            if( ch )
                getch();

            /* Note that cprintf does not convert "\n" to a CR/LF sequence.
             * You can specify this sequence with "\n\r".
             */
            cprintf( "\n\rNum Char ASCII Alpha AlNum Cap Low Digit " );
            cprintf( "XDigit Punct White CTRL Graph Print \n\r" );
        }
        cprintf( "%3d  ", ch );

        /* Console output functions (cprint, cputs, and putch) display
         * graphic characters for all values except 7 (bell), 8 (backspace),
         * 10 (line feed), 13 (carriage return), and 255.
         */
        if( ch == 7 || ch == 8 || ch == 10 || ch == 13 || ch == 255 )
            cprintf("NV" );
        else
            cprintf("%c ", ch );
        cprintf( "%5s", isascii( ch )  ? "Y" : "N" );
        cprintf( "%6s", isalpha( ch )  ? "Y" : "N" );
        cprintf( "%6s", isalnum( ch )  ? "Y" : "N" );
        cprintf( "%5s", isupper( ch )  ? "Y" : "N" );
        cprintf( "%4s", islower( ch )  ? "Y" : "N" );
        cprintf( "%5s", isdigit( ch )  ? "Y" : "N" );
        cprintf( "%7s", isxdigit( ch ) ? "Y" : "N" );
        cprintf( "%6s", ispunct( ch )  ? "Y" : "N" );
        cprintf( "%6s", isspace( ch )  ? "Y" : "N" );
        cprintf( "%5s", iscntrl( ch )  ? "Y" : "N" );
        cprintf( "%6s", isprint( ch )  ? "Y" : "N" );
        cprintf( "%6s\n\r", isgraph( ch )  ? "Y" : "N" );
    }
}
