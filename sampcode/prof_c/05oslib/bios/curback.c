/*
 *	curback -- move cursor back (left); return the
 *	value of the new column position
 */

#include <local\bioslib.h>

int
curback(n, pg)
int n, pg;
{
	int r, c;

	/*
	 *	move the cursor left by up to n positions but 
	 *	not past the beginning of the current line
	 */
	readcur(&r, &c, pg);
	if (c - n < 0)
		c = 0;
	putcur(r, c, pg);
	return (c);
}
