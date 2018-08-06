/*
 *	putfld -- display a string in the prevailing
 *	video attribute while compressing runs of a
 *	single character, and pad the field to full width
 *	with spaces if necessary
 */

int
putfld(s, w, pg)
register char *s;	/* string to write */
int w;			/* field width */
int pg;			/* screen page for writes */
{
	int r, c, cols;
	register int n;

	extern int putcur(int, int, int);
	extern int readcur(int *, int *, int);
	extern int writec(unsigned char, int, int);

	/* get starting (current) position */
	readcur(&r, &c, pg);

	/* write the string */
	for (n = 0; *s != '\0' && n < w; s += cols, n += cols) {
		putcur(r, c + n, pg);
		/* compress runs to a single call on writec() */
		cols = 1;
		while (*(s + cols) == *s && n + cols < w)
			++cols;
		writec(*s, cols, pg);
	}

	/* pad the field, if necessary */
	if (n < w) {
		putcur(r, c + n, pg);
		writec(' ', w - n, pg);
	}

	return (w - n);
}
