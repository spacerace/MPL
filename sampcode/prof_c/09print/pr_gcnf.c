/*
 *	pr_gcnf -- get configuration for pr program
 */

#include <stdio.h>
#include <string.h>
#include <local\std.h>
#include <local\printer.h>
#include "print.h"

/* expected number of configuration items */
#define N_NBR	12

PRINT pcnf;

int
pr_gcnf(pname)
char *pname;
{
	char line[MAXLINE];
	char *s;
	int cnf[N_NBR];
	int n, errcount, good;
	FILE *fp, *fconfig(char *, char *);

	/* get configuration file values, if any */
	n = good = errcount = 0;
	if ((fp = fconfig("CONFIG", "pr.cnf")) != NULL) {
		while (n < N_NBR && (s = fgets(line, MAXLINE, fp)) != NULL) {
			cnf[n] = atoi(s);
			++n;
		}
		if ((s = fgets(line, MAXLINE, fp)) == NULL)
			++errcount;
		else
			strcpy(pcnf.p_dest, strtok(line, " \t\n"));
		if (n != N_NBR)
			++errcount;
		if (errcount == 0)
			good = 1;
		if (fclose(fp) == -1)
			fatal(pname, "cannot close config file");
	}

	/* use config data is good; use defaults otherwise */
	pcnf.p_top1 = good ? cnf[0]: TOP1;
	pcnf.p_top2 = good ? cnf[1] : TOP2;
	pcnf.p_btm = good ? cnf[2] : BOTTOM;
	pcnf.p_wid = good ? cnf[3] : MAXPCOL;
	pcnf.p_lmarg = good ? cnf[4] : MARGIN;
	pcnf.p_rmarg = good ? cnf[5] : MARGIN;
	pcnf.p_len = good ? cnf[6] : PAGELEN;
	pcnf.p_lpi = good ? cnf[7] : LPI;
	pcnf.p_mode = good ? cnf[8] : 0;
	pcnf.p_lnum = good ? cnf[9] : 0;
	pcnf.p_ff = good ? cnf[10] : 0;
	pcnf.p_tabint = good ? cnf[11] : TABSPEC;
	if (!good)
		strcpy(pcnf.p_dest, "PRN");
	if (pcnf.p_mode == 1)
		pcnf.p_font = CONDENSED;
	strcpy(pcnf.p_hdr, "");

	return (errcount);
}
