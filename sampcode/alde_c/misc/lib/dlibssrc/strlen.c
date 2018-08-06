int strlen(string)
register char *string;
/*
 *	Returns the number of characters in a string, not including the
 *	terminating '\0'.
 */
{
	register int n = 0;

	while(*string++)
		++n;
	return(n);
}
