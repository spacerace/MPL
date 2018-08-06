/*
 *	colornum -- return the IBM number for the color
 *	presented as a string; return -1 if no match.
 */

#include <stdio.h>
#include <string.h>
#include <local\ibmcolor.h>

#define NCHARS	3

int
colornum(name)
char *name;
{
	register int n;
	static struct color_st {
		char *c_name;
		int c_num;
	} colortab[] = {
		"black",	IBM_BLACK,
		"blue",		IBM_BLUE,
		"green",	IBM_GREEN,
		"cyan",		IBM_CYAN,
		"red",		IBM_RED,
		"magenta",	IBM_MAGENTA,
		"brown",	IBM_BROWN,
		"white",	IBM_WHITE,
		"normal",	IBM_NORMAL,
		"bright",	IBM_BRIGHT,
		"light",	IBM_BRIGHT,
		"bold",		IBM_BRIGHT,
		"yellow",	IBM_BROWN + IBM_BRIGHT,
		"blink",	IBM_BLINK,
		"reverse",	IBM_REVERSE,
		"invisible",	IBM_INVISIBLE,
		NULL,		(-1)
	};

	(void) strlwr(name);
	for (n = 0; colortab[n].c_name != NULL; ++n)
		if ((strncmp(name, colortab[n].c_name, NCHARS)) == 0)
			return (colortab[n].c_num);
	return (-1);
}
