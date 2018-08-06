/*
 *	last_ch -- return a copy of the last character
 *	before the NUL byte in a string
 */

char
last_ch(s)
char *s;
{
	register char *cp;

	/* find end of s */
	cp = s;
	while (*cp != '\0')
		++cp;

	/* return previous character */
	--cp;
	return (*cp);
}
