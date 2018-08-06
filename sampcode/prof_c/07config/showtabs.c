/*
 *	showtabs -- graphically display tabstop settings
 */

#include <stdio.h>
#include <stdlib.h>
#include <local\std.h>

#define MAXCOL	  80
#define TABWIDTH  8

extern long Highest;

main(argc, argv)
int argc;
char *argv[];
{
	int ch, i;
	int interval, tablist[MAXLINE + 1], *p;
	char *tabstr;
	BOOLEAN errflag, fflag, vflag;
	static char pgm[MAXNAME + 1] = { "showtabs" };

	extern char *getpname(char *, char *);
	extern int getopt(int, char **, char *);
	extern char *optarg;
	extern int optind, opterr;
	extern int mkslist(char *);
	extern int selected(long);
	extern void fixtabs(int);
	extern void vartabs(int *);
	extern int tabstop(int);

	if (_osmajor >= 3)
		getpname(*argv, pgm);

	/* process command-line options */
	errflag = fflag = vflag = FALSE;
	interval = 0;
	while ((ch = getopt(argc, argv, "f:v:")) != EOF) {
		switch (ch) {
		case 'f':
			/* used fixed tabbing interval */
			if (vflag == FALSE) {
				fflag = TRUE;
				interval = atoi(optarg);
			}
			break;
		case 'v':
			/* use list of tabs */
			if (fflag == FALSE) {
				vflag = TRUE;
				strcpy(tabstr, optarg);
			}
			break;
		case '?':
			errflag = TRUE;
			break;
		}
	}
	if (errflag == TRUE) {
		fprintf(stderr, "Usage: %s [-f interval | -v tablist]\n", pgm);
		exit(2);
	}

	/* set the tabstops */
	if (vflag == TRUE) {
		/* user-supplied variable tab list */
		mkslist(tabstr);
		p = tablist;
		for (i = 0; i < MAXLINE && i < Highest; ++i)
			*p++ = selected((long)i + 1) ? i : 0;
		*p = -1;	/* terminate the list */
		vartabs(tablist);
	}
	else if (fflag == TRUE)
		/* user-supplied fixed tabbing interval */
		fixtabs(interval);
	else
		/* hardware default tabbing interval*/
		fixtabs(TABWIDTH);

	/* display current tabs settings */
	for (i = 0; i < MAXCOL; ++i)
		if (tabstop(i))
			fputc('T', stdout);
		else if ((i + 1) % 10 == 0)
			fputc('+', stdout);
		else
			fputc('-', stdout);
	fputc('\n', stdout);

	exit(0);
}
