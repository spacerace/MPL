#include <d:\usr\stdlib\stdio.h>

char *strstr(string, pattern)
register char *string;
register char *pattern;
/*
 *	Return a pointer to the first occurance of <pattern> in <string>.
 *	NULL is returned if <pattern> is not found.
 */
{
	register int plen;
	char *strchr();

	plen = strlen(pattern);
	while(string = strchr(string, *pattern)) {
		if(strncmp(string, pattern, plen) == 0)
			return(string);
		++string;
	}
	return(NULL);
}
