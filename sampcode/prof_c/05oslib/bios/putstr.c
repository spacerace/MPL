/*
 *	putstr -- display a character string in the
 *	prevailing video attribute and return number
 *	characters displayed
 */

int
putstr(s, pg)
register char *s;
unsigned int pg;
{
	unsigned int r, c, c0;

	readcur(&r, &c, pg);
	for (c0 = c; *s != '\0'; ++s, ++c) {
		putcur(r, c, pg);
		writec(*s, 1, pg);
	}
	putcur(r, c, pg);
	return (c - c0);
}
