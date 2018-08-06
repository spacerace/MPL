/*
 *	pr_cpy -- copy input stream to output stream
 */

#include <stdio.h>
#include <string.h>
#include <local\std.h>
#include <local\printer.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <time.h>
#include "print.h"

extern PRINT pcnf;
extern char Pagelist[MAXLINE];
extern long Highest;

int
pr_cpy(fin, fout, fname)
FILE *fin;
FILE *fout;
char *fname;
{
	int errcount = 0;
	unsigned int p_line;	/* page-relative line number */
	long f_line;		/* file-relative line number */
	long f_page;		/* file-relative page number */
	int lnlen;		/* line length */
	char line[MAXLINE];	/* input line buffer */
	struct stat tbuf;	/* file information */
	long ltime;		/* date and time */
	FILE *fnull, *fx;	/* additional output file pointers */

	extern void mkslist(char *);	/* make a selection list */
	extern int selected(long);	/* is item in the list? */
	extern int spaces(int, FILE *);	/* emit string of spaces */
	extern int setfont(int, FILE *);/* set printer font type */
	extern int clrprnt(FILE *);	/* clear special fonts */
	extern int lines(int, FILE *);	/* emit string of blank lines */
	static int fit(int, int);	/* will line fit on page? */
	extern int pr_line(char *, FILE *, unsigned int);

	/* install page selection list, if any */
	if (Pagelist[0] != '\0') {
		/* open the NUL device for dumping output */
		if ((fnull = fopen("NUL", "w")) == NULL) {
			perror("Error opening NUL device");
			exit(1);
		}
		mkslist(Pagelist);
	}
	else
		Highest = BIGGEST;

	/* get date and time stamp */
	if (*fname == '\0')
		/* using stdin -- use today's date and time */
		ltime = time(NULL);
	else {
		if (stat(fname, &tbuf) == -1)
			return (-1);
		/* use file's modification time */
		ltime = tbuf.st_mtime;
	}
	p_line = 0;
	f_line = 1;
	f_page = 1;
	while ((lnlen = pr_getln(line, MAXLINE, fin)) > 0 ) {
		/* if formfeed or no room for line, eject page */
		if (line[0] == '\f' || !fit(lnlen, p_line)) {
			/* to top of next page */
			if (pcnf.p_ff == 0)
				lines(pcnf.p_len - p_line, fx);
			else
				fputc('\f', fx);
			p_line = 0;
		}

		/* if at top of page, print the header */
		if (p_line == 0) {
			if (f_page > Highest)
				break;
			fx = selected(f_page) ? fout : fnull;
			p_line += lines(pcnf.p_top1, fx);
			if (pcnf.p_mode != 0)
				setfont(EMPHASIZED, fx);
			spaces(pcnf.p_lmarg, fx);
			if (*pcnf.p_hdr != '\0') 
				fprintf(fx, "%s  ", pcnf.p_hdr);
			else if (*fname != '\0')
				fprintf(fx, "%s  ", strupr(fname));
			fprintf(fx, "Page %u  ", f_page++);
			fputs(ctime(&ltime), fx);
			++p_line;
			if (pcnf.p_mode != 0)
				setfont(pcnf.p_font, fx);
			p_line += lines(pcnf.p_top2, fx);
		}

		/* OK to output the line */
		if (line[0] != '\f')
			p_line += pr_line(line, fx, f_line++);
	}
	if (ferror(fin) != 0)
		++errcount;
	if (p_line > 0 && p_line < pcnf.p_len)
		if (pcnf.p_ff == 0)
			lines(pcnf.p_len - p_line, fx);
		else
			fputc('\f', fx);

	if (pcnf.p_mode != 0)
		clrprnt(fx);
	return (errcount);
}

/*
 *	fit -- return non-zero value if enough physical
 *	lines are available on the current page to take
 *	the current logical line of text
 */

#define NFLDWIDTH  8	/* width of number field */

static int
fit(len, ln)
int len, ln;
{
	int need, left;	/* physical lines */
	int cols;	/* columns of actual output */
	int lw;		/* displayable line width */

	/* total need (columns -> physical lines) */
	cols = len + (pcnf.p_lnum > 0 ? NFLDWIDTH : 0);
	lw = pcnf.p_wid - pcnf.p_lmarg - pcnf.p_rmarg;
	need = 1 + cols / lw;

	/* lines remaining on page */
	left = pcnf.p_len - ln - pcnf.p_btm;

	return (need <= left ? 1 : 0);
}
