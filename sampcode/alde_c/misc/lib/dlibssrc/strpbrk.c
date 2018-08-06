#include <d:\usr\stdlib\stdio.h>

char *strpbrk(string, set)
register char *string;
register char *set;
/*
 *	Return a pointer to the first occurance in <string> of any
 *	character in <set>.
 */
{
	char *strchr();

	while(*string) {
		if(strchr(set, *string))
			return(string);
		++string;
	}
	return(NULL);
}
