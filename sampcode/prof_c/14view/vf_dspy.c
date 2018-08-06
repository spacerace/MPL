/*
 *	vf_dspy -- display a screen page
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <local\video.h>
#include <local\std.h>
#include <local\bioslib.h>
#include "vf.h"

/* number field width */
#define NFW	8

void
vf_dspy(buf, lp, os, numbers)
DNODE *buf;
register DNODE *lp;
int os;
BOOLEAN numbers;
{
	register int i;
	int j;
	int textwidth;
	char *cp;
	char nbuf[NFW + 1];

	textwidth = Maxcol[Vmode];
	if (numbers == TRUE)
		textwidth -= NFW;
		
	for (i = 0; i < NROWS; ++i) {
		putcur(TOPROW + i, 0, Vpage);
		cp = lp->d_line;
		if (numbers == TRUE) {
			sprintf(nbuf, "%6u", lp->d_lnum);
			putfld(nbuf, NFW, Vpage);
			putcur(TOPROW + i, NFW, Vpage);
		}
		if (os < strlen(cp))
			putfld(cp + os, textwidth, Vpage);
		else
			writec(' ', textwidth, Vpage);
		if (lp == buf->d_prev) {
			++i;
			break;	/* no more displayable lines */
		}
		else
			lp = lp->d_next;
	}

	/* clear and mark any unused lines */
	for ( ; i < NROWS; ++i) {
		putcur(i + TOPROW, 0, Vpage);
		writec(' ', Maxcol[Vmode], Vpage);
		writec('~', 1, Vpage);
	}
	return;
}
