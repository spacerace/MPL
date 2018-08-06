char *strncat(dest, source, limit)
register char *dest;
register char *source;
register int limit;
/*
 *	Concatenate <limit> characters from <source> onto <dest>.  If
 *	<source> contains less than <limit> characters, the length of
 *	source is used for <limit>.  The terminating '\0' is always
 *	added.  A pointer to <dest> is returned.
 */
{
	register char *p = dest;

	while(*dest)
		++dest;
	while((limit--) && (*dest++ = *source++))
		;
	*dest = '\0';
	return(p);
}
