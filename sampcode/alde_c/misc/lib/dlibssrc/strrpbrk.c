#include <d:\usr\stdlib\stdio.h>

char *strrpbrk(string, set)
register char *string;
register char *set;
/*
 *	Return a pointer to the last occurance in <string> of any
 *	character in <set>.
 */
{
	register char *p;
	char *strrchr(), *strchr();

	p = strrchr(string, '\0');		/* start at EOS */
	while(string != p) {
		if(strchr(set, *--p))
			return(p);
	}
	return(NULL);
}
