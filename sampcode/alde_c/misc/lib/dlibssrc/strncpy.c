char *strncpy(dest, source, limit)
register char *dest;
register char *source;
register int limit;
/*
 *	Copies the <source> string to the <dest>.  At most, <limit>
 *	characters are copied.  If <source> ends before <limit> characters
 *	have been copied, the '\0' is copied, otherwise <dest> is not
 *	terminated by the copy.
 */
{
	register char *p = dest;

	while((limit--) && (*dest++ = *source++))
		;
	return(p);
}
