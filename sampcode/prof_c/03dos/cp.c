/*
 *	cp -- a simplified copy command
 */

#include <stdio.h>

main()
{
	int ch;

	while ((ch = getc(stdin)) != EOF)
		putc(ch, stdout);
	exit(0);
}
