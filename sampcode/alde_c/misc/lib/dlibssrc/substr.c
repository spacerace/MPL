char *substr(dest, source, start, end)
register char *dest;
register char *source;
register int start;
register int end;
/*
 *	Copy characters from <source> to <dest> starting with character
 *	<start> and ending with <end>.  A pointer to <dest>, which will
 *	be '\0' terminated, is returned.
 */
{
	register char *p = dest;
	register int n;

	n = strlen(source);
	if(start > n)
		start = n - 1;
	if(end > n)
		end = n - 1;
	source += start;
	while(start++ <= end)
		*p++ = *source++;
	*p = '\0';
	return(dest);
}

char *subnstr(dest, source, start, length)
register char *dest;
register char *source;
register int start;
register int length;
/*
 *	Copy <length> characters from <source> to <dest> starting with
 *	character <start>.  A pointer to <dest>, which will be '\0'
 *	terminated, is returned.
 */
{
	register char *p = dest;
	register int n;

	n = strlen(source);
	if(start > n)
		start = n - 1;
	source += start;
	while(*source && (length--))
		*p++ = *source++;
	*p = '\0';
	return(dest);
}
