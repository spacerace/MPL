/*
    TRYBREAK.C

    Demo of BREAK.ASM Ctrl-Break and Ctrl-C 
    interrupt handler, by Ray Duncan
    
    Build:   MASM /Mx BREAK;
             CL TRYBREAK.C BREAK.OBJ

    Usage:   TRYBREAK

*/

#include <stdio.h>

main(int argc, char *argv[])
{   
    int hit = 0;                    /* flag for keypress */
    int c = 0;                      /* character from keyboard */
    static int flag = 0;            /* true if Ctrl-Break 
                                       or Ctrl-C detected */    

    puts("\n*** TRYBREAK.C running ***\n");
    puts("Press Ctrl-C or Ctrl-Break to test handler,");
    puts("Press the Esc key to exit TRYBREAK.\n");

    capture(&flag);                 /* install new Ctrl-C and
                                       Ctrl-Break handler and
                                       pass address of flag */

    puts("TRYBREAK has captured interrupt vectors.\n");

    while(1)                    
    {   
        hit = kbhit();              /* check for keypress */
                                    /* (MS-DOS sees Ctrl-C
                                       when keyboard polled) */

        if(flag != 0)               /* if flag is true, an */
        {                           /* interrupt has occurred */
            puts("\nControl-Break detected.\n");
            flag = 0;               /* reset interrupt flag */
        }   
        if(hit != 0)                /* if any key waiting */
        {  
            c = getch();            /* read key, exit if Esc */
            if( (c & 0x7f) == 0x1b) break;
            putch(c);               /* otherwise display it */  
        }
    } 
    release();                      /* restore original Ctrl-C
                                       and Ctrl-Break handlers */

    puts("\n\nTRYBREAK has released interrupt vectors.");
}
