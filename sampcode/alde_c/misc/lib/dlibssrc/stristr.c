#include <d:\usr\stdlib\stdio.h>

char *stristr(string, pattern)
register char *string;
register char *pattern;
/*
 *	Same as strstr(), but ignore the case of any alphabetic characters.
 */
{
	register int plen;

	plen = strlen(pattern);
	while(*string) {
		if(strnicmp(string, pattern, plen) == 0)
			return(string);
		++string;
	}
	return(NULL);
}
