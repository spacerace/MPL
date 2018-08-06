/*
 *  menumode -- process user commands interactively
 */

#include <stdio.h>
#include <local\ansi.h>
#include <local\ibmcolor.h>
#include <local\keydefs.h>

/* maximum color number */
#define MAX_CNUM	15

void
menumode()
{
	register int ch;
	int foreground, background, border;
	extern void setattr(POSITION, int);
	extern void sc_cmds(int, int, int);

	/* default attributes */
	foreground = IBM_WHITE;
	background = IBM_BLACK;
	border = IBM_BLACK;

	ANSI_SGR(ANSI_NORMAL);
	setattr(FGND, foreground);
	setattr(BKGND, background);
	ANSI_ED;
	palette(0, border);
	sc_cmds(foreground, background, border);
	while ((ch = getkey()) != K_RETURN) {
		switch (ch) {
		case K_F1:
			/* decrement foreground color */
			if (--foreground < 0)
				foreground = MAX_CNUM;
			break;
		case K_F2:
			/* increment foreground color */
			if (++foreground > MAX_CNUM)
				foreground = 0;
			break;
		case K_F3:
			/* decrement background color */
			if (--background < 0)
				background = MAX_CNUM;
			break;
		case K_F4:
			/* increment background color */
			if (++background > MAX_CNUM)
				background = 0;
			break;
		case K_F5:
			/* decrement border color */
			if (--border < 0)
				border = MAX_CNUM;
			break;
		case K_F6:
			/* increment border color number */
			if (++border > MAX_CNUM)
				border = 0;
			break;
		default:
			continue;
		}
		ANSI_SGR(ANSI_NORMAL);
		setattr(FGND, foreground);
		setattr(BKGND, background);
		palette(0, border);
		ANSI_ED;
		sc_cmds(foreground, background, border);
	}
	return;
}
