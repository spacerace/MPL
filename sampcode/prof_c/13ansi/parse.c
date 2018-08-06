/*
 *	parse -- process a list of attribute specifications
 */

#include <stdio.h>
#include <local\ansi.h>
#include <local\std.h>
#include <local\ibmcolor.h>

/* buffer length for string comparisons */
#define NCHARS	3
#define C_MASK	0x7

void
parse(nargs, argvec)
int nargs;	/* number of argument vectors */
char *argvec[];	/* pointer to the argument vector array */
{
	int i, intensity;
	int attribute;
	POSITION pos;
	char str[NCHARS + 1];
	extern int colornum(char *);
	extern void setattr(POSITION, int);

	/* clear all attributes */
	ANSI_SGR(ANSI_NORMAL);

	/* look for a single attribute specification */
	if (nargs == 2) {
		attribute = colornum(argvec[1]);
		switch (attribute) {
		case IBM_NORMAL:
			palette(0, IBM_BLACK);
			return;
		case IBM_REVERSE:
			ANSI_SGR(ANSI_REVERSE);
			palette(0, IBM_WHITE);
			return;
		case IBM_INVISIBLE:
			ANSI_SGR(ANSI_INVISIBLE);
			return;
		case IBM_BLINK:
			ANSI_SGR(ANSI_BLINK);
			return;
		}
	}

	/* must be separate attribute specifications */
	pos = FGND;
	intensity = 0;
	for (i = 1; i < nargs; ++i) {
		attribute = colornum(argvec[i]);
		if (attribute == -1) {
			ANSI_ED;
			fprintf(stderr, "\nIllegal parameter\n");
			exit (2);
		}
		if (attribute == IBM_BRIGHT) {
			intensity = IBM_BRIGHT;
			continue;
		}
		setattr(pos, attribute | intensity);
		if (pos == FGND)
			pos = BKGND;
		else if (pos == BKGND)
			pos = BDR;
		intensity = 0;
	}
	return;
}
