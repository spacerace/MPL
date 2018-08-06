char *strset(string, c)
char *string;
register char c;
/*
 *	Fill <string> with <c> up the the terminating '\0' of <string>.
 */
{
	register char *p = string;

	while(*p)
		*p++ = c;
	return(string);
}

char *strnset(string, c, n)
char *string;
register char c;
register int n;
/*
 *	Fill at most <n> characters of <string> with <c>, up the the
 *	terminating '\0' of <string>.
 */
{
	register char *p = string;

	while(n-- && *p)
		*p++ = c;
	return(string);
}
