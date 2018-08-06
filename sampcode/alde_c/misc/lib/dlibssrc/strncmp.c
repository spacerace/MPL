int strncmp(str1, str2, limit)
register char *str1;
register char *str2;
register int limit;
/*
 *	Compare strings as with strcmp(), but limit comparison to the
 *	<limit> characters.
 */
{
	for(; ((--limit) && (*str1 == *str2)); ++str1, ++str2)
		if(*str1 == '\0')
			return(0);
	return(*str1 - *str2);
}
