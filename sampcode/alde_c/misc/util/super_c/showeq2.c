#include "eqlib.h"

/*      main()

        Function: Get and display the system memory size and the
        equipment configuration.

        Algorithm: Call the ROM BIOS interface routines memSz and
        getEq. Then interpret their replies and display the results.
*/

main()

{
        int theEq;      /* The equipment configuration. */
        int i;

        /* Clear the display. */
        scrUp(0,0,0,24,79,0);

        /* Display the memory size at the bottom of the screen. */
        setPos(0,15,30);
        printf("Memory size is %dK.",memSz());

        /* Display the equipment configuration at the top of the screen. */
        setPos(0,0,0);
        printf("Equipment present:\n   ");
        /* Get the equipment configuration. */
        theEq = getEq();
        /* Get the number of printers. */
        i = (theEq & prtnMask) >> prtnShft;
        /* If there are any at all, show how many. */
        if (i != 0) printf("%d printer(s).\n   ",i);
        /* If there's a game I/O interface present, say so. */
        if (theEq & gioMask) printf("1 game I/O.\n   ");
        /* Get the number of serial ports. */
        i = (theEq & RS232Mask) >> RS232Shft;
        /* If there are any, show how many. */
        if (i != 0) printf("%d serial port(s).\n   ",i);
        /* If there are any diskette drives present, show how many. */
        if (theEq & diskMask) printf("%d diskette drive(s).\n   ",
                                     ((theEq & dsknMask) >> dsknShft)+1);
        /* Show the initial video mode. */
        printf("Initial video mode: ");
        i = theEq & vmodeMask;
        if (i == CO40) printf("40x25 color");
        else if (i == CO80) printf("80x25 color");
        else if (i == MONO) printf("80x25 monochrome");
        /* Show the system board RAM size. */
        printf(".\n   System board RAM size: %dK bytes.\n",
               16*(((theEq & sbramMask) >> sbramShft)+1));

        /* Return the cursor to the bottom of the page. */
        setPos(0,23,0);
}

