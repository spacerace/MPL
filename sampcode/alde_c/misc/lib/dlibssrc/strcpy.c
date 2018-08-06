char *strcpy(dest, source)
register char *dest;
register char *source;
/*
 *	Copies the <source> string to the <dest> including the '\0'.  A
 *	pointer to the start of <dest> is returned.
 */
{
	register char *p = dest;

	while(*dest++ = *source++)
		;
	return(p);
}
