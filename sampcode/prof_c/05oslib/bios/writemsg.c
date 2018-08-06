/*
 *	writemsg -- displays a message in a field of the prevailing
 *	video attribute and returns the number of displayable message
 *	characters written; truncates the message if its too long
 *	to fit in the field
 */

#include <stdio.h>
#include <local\std.h>

int writemsg(r, c, w, s1, s2, pg)
int r, c, w ;
char *s1, *s2;
int pg;
{
	int n = 0;
	char *cp;

	/* display first part of the message */
	if (s1 != NULL)
		for (cp = s1; *cp != '\0' && n < w; ++n, ++cp) {
			putcur(r, c + n, pg);
			writec(*cp, 1, pg);
		}

	/* display second part of the message */
	if (s2 != NULL)
		for (cp = s2; *cp != '\0' && n < w; ++n, ++cp) {
			putcur(r, c + n, pg);
			writec(*cp, 1, pg);
		}

	/* pad the remainder of the field, if any, with spaces */
	if (n < w) {
		putcur(r, c + n, pg);
		writec(' ', w - n, pg);
	}

	return (n);
}
