#include <stdio.h>

char *fgets(data, limit, fp)
char *data;
register int limit;
register FILE *fp;
/*
 *	Get data from <fp> and puts it in the <data> buffer.  At most,
 *	<limit>-1 characters will be read.  Input will also be terminated
 *	when a newline is read.  <data> will be '\0' terminated and the
 *	newline, if read, will be included.  A pointer to the start of
 *	<data> is returned, or NULL for EOF.
 */
{
	register char *p = data;
	register int c;

	while((--limit > 0) && ((c = fgetc(fp)) != EOF))
		if((*p++ = c) == '\n')
			break;
	*p = '\0';
	return((c == EOF && p == data) ? NULL : data);	/* NULL == EOF */
}
