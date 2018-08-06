/* HARDERR.C illustrates handling of hardware errors using functions:
 *      _harderr            _hardresume         _hardretn
 */

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <direct.h>
#include <string.h>
#include <dos.h>
#include <bios.h>

void far hardhandler( unsigned deverr, unsigned doserr, unsigned far *hdr );
int _bios_str( char *p );

main() 
{
    /* Install our hard error handler. */
    _harderr( hardhandler );

    /* Test it. */
    printf( "Make sure there is no disk in drive A:\n" );
    printf( "Press a key when ready...\n" );
    getch();
    if( mkdir( "a:\test" ) )
    {
        printf( "Failed" );
        exit( 1 );
    }
    else
    {
        printf( "Succeeded" );
        rmdir( "a:test" );
        exit( 0 );
    }
}

/* Handler to deal with hard error codes. Since DOS is not reentrant,
 * it is not safe use DOS calls to do I/O within the DOS Critical Error
 * Handler (int 24h) used by _harderr. Therefore, screen output and
 * keyboard input must be done through the BIOS.
 */
void far hardhandler( unsigned deverr, unsigned doserr, unsigned far *hdr )
{
    int ch;
    static char buf[200], tmpbuf[10];

    /* Copy message to buffer, then use BIOS to print it. */
    strcpy( buf, "\n\rDevice error code: " );
    strcat( buf, itoa( deverr, tmpbuf, 10 ) );
    strcat( buf, "\n\rDOS error code:    " );
    strcat( buf, itoa( doserr, tmpbuf, 10 ) );
    strcat( buf, "\n\r(R)etry, (F)ail, or (Q)uit? " );

    /* Use BIOS to write strings and get a key. */
    _bios_str( buf );
    ch = _bios_keybrd( _KEYBRD_READ ) & 0x00ff;
    _bios_str( "\n\r" );

    switch( ch )
    {
        case 'R':
        case 'r':       /* Try again */
        default:
            _hardresume( _HARDERR_RETRY );
        case 'F':
        case 'f':       /* Return to DOS with error code */
            _hardretn( doserr );
        case 'Q':
        case 'q':       /* Quit program */
            _hardresume( _HARDERR_ABORT );

    }
}

/* Display a string using BIOS interrupt 0x0e (Write TTY). Return length
 * of string displayed.
 */
int _bios_str( char *p )
{
    union REGS inregs, outregs;
    char *start = p;

    inregs.h.ah = 0x0e;
    for( ; *p; p++ )
    {
        inregs.h.al = *p;
        int86( 0x10, &inregs, &outregs );
    }
    return p - start;
}
