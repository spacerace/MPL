char *strcat(dest, source)
register char *dest;
register char *source;
/*
 *	Concatenate <source> on the end of <dest>.  The terminator of
 *	<dest> will be overwritten by the first character of <source>.
 *	The termintor from <source> will be copied.  A pointer to
 *	the modified <dest> is returned.
 */
{
	register char *p = dest;

	while(*dest)
		++dest;
	while(*dest++ = *source++)
		;
	return(p);
}
