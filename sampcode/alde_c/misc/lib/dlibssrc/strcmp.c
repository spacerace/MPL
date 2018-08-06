int strcmp(str1, str2)
register char *str1;
register char *str2;
/*
 *	Lexicographically compare the two strings.  Return a value
 *	indicating the relationship between the strings.  Possible
 *	return values are:
 *		negative	str1 < str2
 *		0		str1 == str2
 *		positive	str1 > str2
 */
{
	for(; *str1 == *str2; ++str1, ++str2)
		if(*str1 == '\0')
			return(0);
	return(*str1 - *str2);
}
