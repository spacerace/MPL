/*                      Terminal Emulator Program with Printing
*/

#include "slib.h"

#define TRUE 1
#define FALSE 0

/*      main()

        Function: Display data coming from COM1 on the screen, and send
        keystrokes from the keyboard out to COM1. If F1 is pressed,
        toggle into print-also mode, where each input is also sent to
        the printer port.

        Algorithm: Loop, waiting for data from either side to appear.
        Use keyChk to see if there's anything from the keyboard; if
        there is, send it using serSend. Use serRecv to check if
        there's anything from the serial port; if there is, display
        it using putTty, and if we're in print mode, also print it
        using prtPrint.
*/

main()

{
        int ch;         /* Character to transfer. */
        int printOn;    /* TRUE if printing is toggled on. */

        /* Initialize the printer. */
        prtInit(0);
        printOn = FALSE;

        /* Initialize the serial port. */
        serInit(0,B1200+DB8+SB1+PNONE);

        /* Main loop. */
        while (TRUE) {
                /* Check for keyboard input. */
                if (keyChk(&ch)) {
                        /* If yes, clear the character from the queue. */
                        keyRd();
                        /* Check for non-ASCII. */
                        if ((ch & 0xFF) == 0) {
                                /* Check for F1 key. */
                                if (((ch>>8) & 0xFF) == 59) 
                                        /* If F1, toggle print mode. */
                                        printOn = !printOn;
                                else break;
                        };
                        /* Send the character out the serial port. */
                        serSend(0,ch);
                };
                /* Anything available from the serial port? */
                if (serStat(0) & RCVDRDY) {
                        /* If so, read it in. */
                        ch = serRecv(0);
                        /* And put it on the screen. */
                        putTty(0,ch,3);
                        /* If we're in print mode, print it as well. */
                        if (printOn) prtPrint(0,ch);
                };
        };
}

