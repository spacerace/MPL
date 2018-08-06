/*              Change Border Color from Keyboard

        This program changes the background color/attributes.
*/

/*      main(argc,argv)

        Function: Watch the keyboard and change the border color of the
        display based on the current state of the shift, control and
        alt keys. Exit when any other key is pressed.

        Algorithm: Repeatedly call keyChk to see if something other than
        shift keys are pressed. If not, call keyShf to get the current
        state of the shift keys, and pass that on to setPal to actually
        set the color o the border.
*/

main(argc,argv)

int argc;
char *argv[];

{
        int key;

        /* Repeatedly call keyChk to see if anything non-shift was pressed. */
        while (!keyChk(&key)) 
                /* If not, set the border to the current shift state. */
                setPal(0,keyShf());
}

