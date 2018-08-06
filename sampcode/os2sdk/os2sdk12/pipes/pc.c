/*
 * Child program for pipes example. This program
 * is a filter which transliterates lower case
 * characters into upper case and passes others
 * through unchanged.
 *
 * Created by Microsoft Corp. 1987
 */

#define INCL_DOSPROCESS

#include <os2def.h>
#include <bsedos.h>
#include <stdio.h>

main()
{
	int c;

	while((c = getchar()) != EOF)
		putchar(toupper(c));
	printf("Child saw an EOF\n");
	fflush(stdout);
	DosExit(EXIT_THREAD,0);
}
