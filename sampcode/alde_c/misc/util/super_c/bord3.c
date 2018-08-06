/*              Change Border Color from Keyboard

        This program changes the background color/attributes.
*/

#define TRUE 1
#define FALSE 0

/*      main(argc,argv)

        Function: Watch the keyboard and change the attributes of each
        character on the display based on the current state of the shift, 
        control and alt keys. The low four bits of the attributes are to
        be set to the current shift state; the upper four bits are set from
        the shift state when the space bar is pressed. Exit when any key 
        other than the space bar is pressed.

        Algorithm: Repeatedly call keyChk to see if something other than
        shift keys are pressed. If not, call keyShf to get the current
        state of the shift keys; if they've changed, call getACh/putACh
        for each character on the display.
*/

main(argc,argv)

int argc;
char *argv[];

{
        int key;                        /* Key pressed. */
        char curAttr, lastAttr;         /* Current and last attribute. */
        char ch, attr;                  /* Char and attribute from display. */
        int i,j;                        /* Row and column indices. */

        /* Set the attribute to white on black. */
        lastAttr = 7;

        /* Repeatedly call keyChk to see if anything non-shift was pressed. */
        while (TRUE) {
                /* Assume the top bits stay the same as they were. */
                curAttr = lastAttr & 0xF0;
                /* Check for a key press. */
                if (keyChk(&key)) {
                        /* If yes, check for the space bar. */
                        if ((key & 0xFF) == ' ') {
                                /* If yes, clear the key from the queue. */
                                keyRd();
                                /* Set the new top of the attribute. */
                                curAttr = (keyShf() << 4) & 0xF0;
                        /* Otherwise, exit the loop. */
                        } else break;
                };
                curAttr |= keyShf() & 0xF;
                /* If we've got a new attribute, set all the characters
                   to it. */
                if (curAttr != lastAttr)
                        for (i = 0; i < 24; i++)
                                for (j = 0; j < 79; j++) {
                                        /* Position the cursor. */
                                        setPos(0,i,j);
                                        /* Get the old character/attribute. */
                                        getACh(0,&ch,&attr);
                                        /* Put the new character/attribute. */
                                        putACh(0,ch,curAttr,1);
                                        /* Remember the new attribute. */
                                        lastAttr = curAttr;
                                };
         };
}

