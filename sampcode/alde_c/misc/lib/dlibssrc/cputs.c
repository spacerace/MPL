#include <stdio.h>

extern	int	putch();

void cputs(data)
register char *data;
/*
 *	Write the characters in <data> directly to the console using the
 *	system dependent putch() function.  A newline WILL NOT be written
 *	after the data.
 */
{
	while(*data)
		putch(*data++);
}
