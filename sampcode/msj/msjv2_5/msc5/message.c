/*

Figure 6
========

*/

/***************************************************************
* Message
*
* DESCRIPTION:  Display the message text at the specified
*  screen location (row, col).  If wait has a non-zero value,
*  wait for the user to press a key.  When the user complies,
*  grab the character from the keyboard buffer so it won't
*  interfere with the calling program following the return.
***************************************************************/

#include <conio.h>
#include <graph.h>

void
Message(row, col, text, wait)
short row, col; /* text position */
char *text;     /* text pointer */
short wait;     /* wait flag */
		/* (wait != 0 means wait for a keypress) */
{
    int k;      /* key code */

    /*
     * Write the prompt text at the specified location.
     */
    _settextposition(row, col);
    _outtext(text);

    /*
     * If the wait flag is set, wait for a key to be pressed,
     * then remove the code from the keyboard buffer.  Handle
     * extended codes by grabbing two bytes if the first is NUL.
     */
    if (wait) {
	while (!kbhit())
	    ;
	k = getch();            /* read the character */
	if (k == '\0')
	    /* extended code -- get next byte */
	    getch();
    }
}
