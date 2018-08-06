/*
 *	writestr -- write a string in
 *	the prevailing video attribute
 */

#include <local\std.h>

int
writestr(s, pg)
register char *s;	/* string to write */
unsigned int pg;	/* screen page for writes */
{
	unsigned int r, c, c0;

	readcur(&r, &c, pg);
	for (c0 = c; *s != '\0'; ++s, ++c) {
		putcur(r, c, pg);
		writec(*s, 1, pg);
	}

	/* restore cursor position and return # of characters displayed */
	putcur(r, c0, pg);
	return (c - c0);
}
