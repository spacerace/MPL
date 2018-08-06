/*
 *	ansi_tst -- verify that the ANSI.SYS driver is loaded
 *	(prints message and exits is ANSI driver not working)
 */

#include <stdio.h>
#include <local\ansi.h>
#include <local\video.h>

#define TST_ROW	2
#define TST_COL	75

void
ansi_tst()
{
	int row, col;
	static char *help[] = {
		"\n",
		"ANSI.SYS device driver not loaded:\n",
		"  1. Copy ANSI.SYS to your system disk.\n",
		"  2. Add the line device=ansi.sys to your\n",
		"     CONFIG.SYS file and reboot your machine.\n",
		NULL
	};
	char **msg;

	extern int getstate();
	extern int readcur(int *, int *, int);

	getstate();
	ANSI_CUP(TST_ROW, TST_COL);
	readcur(&row, &col, Vpage);
	if (row != TST_ROW - 1 || col != TST_COL - 1) {
		for (msg = help; *msg != NULL; ++msg)
			fputs(*msg, stderr);
		exit(1);
	}

	return;
}
