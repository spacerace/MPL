char *strtrim(string, junk)
register char *string;
register char *junk;
/*
 *	Remove leading and trailing characters found in <junk>
 *	from <string>.  Return a pointer to the modified <string>.
 */
{
	register char *p = string, *q = string;
	char *strchr();

	while(*string && strchr(junk, *string))
		++string;
	while(*string && !strchr(junk, *string))
		*p++ = *string++;
	*p = '\0';
	return(q);
}
