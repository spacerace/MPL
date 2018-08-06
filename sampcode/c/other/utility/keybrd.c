/* KEYBRD.C illustrates:
 *      _bios_keybrd
 */

#include <bios.h>
#include <stdio.h>
#include <ctype.h>

main()
{
    unsigned key, shift;
    unsigned char scan, ascii = 0;

    /* Read and display keys until ESC is pressed. */
    while( ascii != 27 )
    {
        /* Drain any keys in the keyboard type-ahead buffer, then get
         * the current key. If you want the last key typed rather than
         * the key currently being typed, omit the initial loop.
         */
        while( _bios_keybrd( _KEYBRD_READY ) )
            _bios_keybrd( _KEYBRD_READ );
        key = _bios_keybrd( _KEYBRD_READ );

        /* Get shift state. */
        shift = _bios_keybrd( _KEYBRD_SHIFTSTATUS );

        /* Split key into scan and ascii parts. */
        scan = key >> 8;
        ascii = key & 0x00ff;

        /* Categorize key. */
        if( ascii )
            printf( "ASCII: yes\tChar: %c \t",
                    isgraph( ascii ) ? ascii : ' ' );
        else
            printf( "ASCII: no\tChar: NA\t" );
        printf( "Code: %.2X\tScan: %.2X\t Shift: %.2X\n",
                ascii, scan, shift );
    }
}
