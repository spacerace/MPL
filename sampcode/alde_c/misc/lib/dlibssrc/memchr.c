#include <stdio.h>

char *memchr(buf, c, cnt)
register char *buf;
register char c;
register int cnt;
/*
 *	Search the first <cnt> bytes of <buf> for <c>.  Returns a pointer to
 *	the matching character, or NULL if not found.
 */
{
	while(cnt--)
		if(*buf++ == c)
			return(--buf);
	return(NULL);
}
