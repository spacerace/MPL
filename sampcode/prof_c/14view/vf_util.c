/*
 *	vf_util -- utility functions for ViewFile
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <local\std.h>
#include <local\video.h>
#include <local\keydefs.h>
#include "vf.h"

extern int Startscan, Endscan;

#define NDIGITS	6

/*
 *	gotoln -- jump to an absolute line number
 */

DNODE *
gotoln(buf)
DNODE *buf;
{
	register int ln;
	register DNODE *lp;
	char line[NDIGITS + 1];

	extern void showmsg(char *);
	extern char *getstr(char *, int);

	/* get line number from user */
	showmsg("Line number: ");
	setctype(Startscan, Endscan);			/* cursor on */
	ln = atoi(getstr(line, NDIGITS + 1));
	setctype(MAXSCAN, MAXSCAN);			/* cursor off */

	/* check boundary conditions */
	if (ln > buf->d_prev->d_lnum || ln <= 0) {
		showmsg("Line out of range");
		return ((DNODE *)NULL);
	}

	/* find the line */
	for (lp = buf->d_next; ln != lp->d_lnum; lp = lp->d_next)
		;
	return (lp);
}


/*
 *	showhelp -- display a help frame
 */

#define HELPROW	TOPROW + 3
#define HELPCOL	10
#define VBORDER	1
#define HBORDER	2

void
showhelp(textattr)
unsigned char textattr;	/* attribute of text area */
{
	register int i, n;
	int nlines, ncols;
	unsigned char helpattr;
	static char *help[] = {
		"PgUp (U)        Scroll up the file one screen page",
		"PgDn (D)        Scroll down the file one screen page",
		"Up arrow (-)    Scroll up in the file one line",
		"Down arrow (+)  Scroll down in the file one line",
		"Right arrow (>) Scroll right by 20 columns",
		"Left arrow (<)  Scroll left by 20 columns",
		"Home (B)        Go to beginning of file buffer",
		"End (E)         Go to end of file buffer",
		"Alt-g (G)       Go to a specified line in the buffer",
		"Alt-h (H or ?)  Display this help frame",
		"Alt-n (N)       Toggle line-numbering feature",
		"\\ (R)           Reverse search for a literal text string",
		"/ (S)           Search forward for a literal text string",
		"Esc             Next file from list (quits if none)",
		"Alt-q (Q)       Quit",
		"--------------------------------------------------------",
		"            << Press a key to continue >>",
		(char *)NULL
	};

	/* prepare help window */
	ncols = 0;
	for (i = 0; help[i] != (char *)NULL; ++i)
		if ((n = strlen(help[i])) > ncols)
			ncols = n;
	nlines = i - 1;
	--ncols;
	helpattr = (RED << 4) | BWHT;
	clrw(HELPROW - VBORDER, HELPCOL - HBORDER,
		HELPROW + nlines + VBORDER, HELPCOL + ncols + HBORDER,
		helpattr);
	drawbox(HELPROW - VBORDER, HELPCOL - HBORDER,
		HELPROW + nlines + VBORDER, HELPCOL + ncols + HBORDER,
		Vpage);

	/* display the help text */
	for (i = 0; help[i] != (char *)NULL; ++i) {
		putcur(HELPROW + i, HELPCOL, Vpage);
		putstr(help[i], Vpage);
	}

	/* pause until told by a keypress to proceed */
	getkey();

	/* restore help display area to the text attribute */
	clrw(HELPROW - VBORDER, HELPCOL - HBORDER,
		HELPROW + nlines + VBORDER, HELPCOL + ncols + HBORDER,
		textattr);
}
