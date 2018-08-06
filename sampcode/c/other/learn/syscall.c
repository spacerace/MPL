/* SYSCALL.C illustrates system calls to DOS and BIOS interrupts using
 * functions:
 *      intdos          intdosx         bdos
 *      int86           int86x          segread
 *
 * The int86x call is not specifically shown in the example, but it is
 * the same as intdosx except that an interrupt number must be supplied.
 */

#include <dos.h>
#include <stdio.h>
#define LPT1 0

union REGS inregs, outregs;
struct SREGS segregs;

main()
{
    const char far *buffer = "Dollar-sign terminated string\n\r$";
    char far *p;

    /* Print a $-terminated string on the screen using DOS function 0x9. */
    inregs.h.ah = 0x9;
    inregs.x.dx = FP_OFF( buffer );
    segregs.ds = FP_SEG( buffer );
    intdosx( &inregs, &outregs, &segregs );

    /* Get DOS version using DOS function 0x30. */
    inregs.h.ah = 0x30;
    intdos( &inregs, &outregs );
    printf( "\nMajor: %d\tMinor: %d\tOEM number: %d\n\n",
            outregs.h.al, outregs.h.ah, outregs.h.bh );

    segread( &segregs );
    printf( "Segments:\n\tCS\t%.4x\n\tDS\t%.4x\n\tES\t%.4x\n\tSS\t%.4x\n\n",
            segregs.cs, segregs.ds, segregs.es, segregs.ss );

    /* Make sure printer is available. Fail if any error bit is on,
     * or if either operation bit is off.
     */
    inregs.h.ah = 0x2;
    inregs.x.dx = LPT1;
    int86( 0x17, &inregs, &outregs );
    if(  (outregs.h.ah & 0x29) || !(outregs.h.ah & 0x80) ||
                                  !(outregs.h.ah & 0x10) )
        printf( "Printer not available." );
    else
    {
        /* Output a string to the printer using DOS function 0x5. */
        for( p = buffer; *p != '$'; p++ )
            bdos( 0x05, *p, 0 );

        /* Do print screen. */
        inregs.h.ah = 0x05;
        int86( 0x05, &inregs, &outregs );
    }
}
