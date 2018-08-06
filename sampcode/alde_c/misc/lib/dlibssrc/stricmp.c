int stricmp(str1, str2)
register char *str1;
register char *str2;
/*
 *	Compare strings as with strcmp(), but ignore the case of any
 *	alphabetic characters.
 */
{
	register char c1, c2;

	while((c1 = tolower(*str1++)) == (c2 = tolower(*str2++)))
		if(c1 == '\0')
			return(0);
	return(c1 - c2);
}
