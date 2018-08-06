/*
 *	setattr -- execute an attribute update
 */

#include <stdio.h>
#include <local\ansi.h>
#include <local\ibmcolor.h>

#define C_MASK	0x7

void
setattr(pos, attr)
POSITION pos;	/* attribute position */
int attr;	/* composite attribute number (base attr | intensity) */
{
	static int ibm2ansi[] = {
		ANSI_BLACK, ANSI_BLUE, ANSI_GREEN, ANSI_CYAN,
		ANSI_RED, ANSI_MAGENTA, ANSI_BROWN, ANSI_WHITE
	};

	switch (pos) {
	case FGND:
		if (attr & IBM_BRIGHT)
			ANSI_SGR(ANSI_BOLD);
		ANSI_SGR(ibm2ansi[attr & C_MASK] + ANSI_FOREGROUND);
		break;
	case BKGND:
		if (attr & IBM_BRIGHT)
			ANSI_SGR(ANSI_BLINK);
		ANSI_SGR(ibm2ansi[attr & C_MASK] + ANSI_BACKGROUND);
		break;
	case BDR:
		palette(0, attr);
		break;
	}
	return;
}
