/*
 *	pr_line -- ouput a buffered logical line and
 *	return a count of physical lines produced
 */

#include <stdio.h>
#include <stdlib.h>
#include <local\std.h>
#include "print.h"

int
pr_line(s, fout, rline)
char *s;		/* buffered line of text */
FILE *fout;		/* output stream */
unsigned int rline;	/* file-relative line number */
{
	int c_cnt;	/* character position in output line */
	int nlines;	/* number of lines output */
	extern PRINT pcnf;

	extern int spaces(int, FILE *);	/* emit string of spaces */

	nlines = 1;
	c_cnt = 0;

	/* output the left indentation, if any */
	c_cnt += spaces(pcnf.p_lmarg, fout);

	/* output the line number if numbering enabled */
	if (pcnf.p_lnum != 0)
		c_cnt += fprintf(fout, "%6u  ", rline);

	/* output the text of the line */
	while (*s != '\0') {
		if (c_cnt > (pcnf.p_wid - pcnf.p_rmarg)) {
			fputc('\n', fout);
			++nlines;
			c_cnt = 0;
			c_cnt = spaces(pcnf.p_lmarg, fout);
		}
		fputc(*s, fout);
		++s;
		++c_cnt;
	}

	return (nlines);
}
