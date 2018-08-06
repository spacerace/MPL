/*
 *	ansi_cpr -- report where the cursor is located
 *	The position information is placed in the keyboard buffer
 *	in the form ESC[rr;ccR where ESC is the value of the
 *	ESCAPE character (\033) and r and c represent
 *	decimal values of row and column data.
 */

#include <local\ansi.h>

void
ansi_cpr(row, col)
int	*row,
	*col;
{
	int i;

	/* request a cursor position report */
	ANSI_DSR;

	/* toss the ESC and '[' */
	(void) getkey();
	(void) getkey();

	/* read the row number */
	*row = 10 * (getkey() - '0');
	*row = *row + getkey() - '0';
	
	/* toss the ';' separator */
	(void) getkey();
	
	/* read the column number */
	*col = 10 * (getkey() - '0');
	*col = *col + getkey() - '0';

	/* toss the trailing ('R') and return */
	(void) getkey();
	(void) getkey();
	return;
}
