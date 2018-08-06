#include <stdio.h>

char *gets(data)
register char *data;
/*
 *	Get data from stdin and puts it in the <data> buffer.  Input is
 *	terminated when a newline is read.  The	newline will be replaced
 *	by a '\0' to terminate the string.  A pointer to the start of
 *	<data> is returned, or NULL for EOF.
 */
{
	register char *p = data;
	register int c;

	while(((c = fgetc(stdin)) != EOF) && (c != '\n')) {
		if(c == '\b') {
			if(p > data)
				--p;
		}
		else
			*p++ = c;
	}
	*p = '\0';
	return(((c == EOF) && (p == data)) ? NULL : data);  /* NULL == EOF */
}
