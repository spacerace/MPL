/* MORE.C illustrates line input and output using:
 *      gets            puts            isatty          fileno
 *
 * Like the DOS MORE command, it is a filter whose input and output can
 * be redirected.
 */

#include <stdio.h>
#include <bios.h>
#include <io.h>

main()
{
    long line = 0, page = 0;
    char tmp[128];

    /* Get each line from standard input and write to standard output. */
    while( 1 )
    {
        /* If standard out is the screen, wait for key after each screen. */
        if( isatty( fileno( stdout ) ) )
        {
            /* Wait for key every 23 lines. You must get the key directly
             * from the BIOS, since input is usually redirected.
             */
            if( !(++line % 23 ) )
                _bios_keybrd( _KEYBRD_READ );
        }
        /* Must be redirected to file, so send a header every 58 lines. */
        else
        {
            if( !(line++ % 58) )
                printf( "\f\nPage: %d\n\n", ++page );
        }

        /* Get and put a line of text. */
        if( gets( tmp ) == NULL )
            break;
        puts( tmp );

    }
}
