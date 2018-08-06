#include <stdio.h>

extern	int	putch();

cprintf(fmt, arg)
char *fmt;
int arg;
/*
 *	Formatted output directly to the console.  This functions uses the
 *	system dependent putch() for output.  See the _printf() function
 *	for a description of the <fmt> formatting string.
 */
{
	return(_printf(NULL, putch, fmt, &arg));
}
