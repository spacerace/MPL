/*                      Terminal Emulator Program
*/

#define SERCONFIG 0x83  /* 1200 baud, 8 bits, 1 stop bit, no parity. */

#define SERDRDY 0x100   /* Serial receiver data ready bit mask. */

#define TRUE 1
#define FALSE 0

/*      main()

        Function: Display data coming from COM1 on the screen, and send
        keystrokes from the keyboard out to COM1.

        Algorithm: Loop, waiting for data from either side to appear.
        Use keyChk to see if there's anything from the keyboard; if
        there is, send it using serSend. Use serRecv to check if
        there's anything from the serial port; if there is, display
        it using putTty.
*/

main()

{
        int ch; /* Character to transfer. */

        /* Initialize the port. */
        serInit(0,SERCONFIG);

        /* Main loop. */
        while (TRUE) {
                /* Check for keyboard input. */
                if (keyChk(&ch)) {
                        /* If yes, clear the character from the queue. */
                        keyRd();
                        /* Check for non-ASCII. */
                        if ((ch & 0xFF) == 0) {
                                break;
                        };
                        /* Send the character out the serial port. */
                        serSend(0,ch);
                };
                /* Anything available from the serial port? */
                if (serStat(0) & SERDRDY) {
                        /* If so, read it in. */
                        ch = serRecv(0);
                        /* And put it on the screen. */
                        putTty(0,ch,3);
                };
        };
}

