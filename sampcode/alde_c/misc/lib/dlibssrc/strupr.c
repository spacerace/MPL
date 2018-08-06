#include <d:\usr\stdlib\stdio.h>

char *strupr(string)
register char *string;
/*
 *	Convert all alphabetic characters in <string> to upper case.
 */
{
	register char *p = string;

	while(*string) {
		if(islower(*string))
			*string ^= 0x20;
		++string;
	}
	return(p);
}
