/*
 *	nlerase -- replace the first newline in a string
 *	with a null character
 */

char *
nlerase(s)
char *s;
{
	register char *cp;

	cp = s;
	while (*cp != '\n' && *cp != '\0')
		++cp;
	*cp = '\0';

	return (s);
}
