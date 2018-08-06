#include <d:\usr\stdlib\stdio.h>

char *strlwr(string)
register char *string;
/*
 *	Convert all alphabetic characters in <string> to lower case.
 */
{
	register char *p = string;

	while(*string) {
		if(isupper(*string))
			*string ^= 0x20;
		++string;
	}
	return(p);
}
