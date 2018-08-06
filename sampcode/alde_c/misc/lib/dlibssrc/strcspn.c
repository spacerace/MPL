int strcspn(string, set)
register char *string;
register char *set;
/*
 *	Return the length of the sub-string of <string> that consists
 *	entirely of characters not found in <set>.  The terminating '\0'
 *	in <set> is not considered part of the match set.  If the first
 *	character if <string> is in <set>, 0 is returned.
 */
{
	register int n = 0;
	char *strchr();

	while(*string && !strchr(set, *string++))
		++n;
	return(n);
}
