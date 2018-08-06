/*
 *	pr -- file printer
 */

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <local\std.h>
#include "print.h"

char Pagelist[MAXLINE];

main(argc, argv)
int argc;
char **argv;
{
	int ch;
	BOOLEAN errflag;
	extern PRINT pcnf;
	static char pgm[MAXNAME + 1] = { "pr" };

	extern char *getpname(char *, char *);
	extern int getopt(int, char **, char *);
	extern char *optarg;
	extern int optind, opterr;
	extern int pr_gcnf(char *);
	extern pr_file(char *, int, char **);
	extern void pr_help(char *);
	extern void fixtabs(int);
	extern int setprnt();

	if (_osmajor >= 3)
		getpname(*argv, pgm);

	/* do configuration */
	if (pr_gcnf(pgm) != 0) {
		fprintf(stderr, "%s: Configuration error", pgm);
		exit(2);
	}
	if (setprnt() == -1) {
		fprintf(stderr, "%s: Bad printer configuration\n", pgm);
		exit(1);
	}
	fixtabs(pcnf.p_tabint);

	/* process command-line arguments */
	while ((ch = getopt(argc, argv, "efgh:l:no:ps:w:")) != EOF) {
		switch (ch) {
                case 'e':
			/* force "Epson-compatible " printer mode */
			pcnf.p_mode = 1;
			break;
		case 'f':
			/* use formfeed to eject a page */
			pcnf.p_ff = 1;
			break;
                case 'g':
			/* force "generic" printer mode */
			pcnf.p_mode = 0;
			break;
		case 'h':
			/* use specified header */
			strcpy(pcnf.p_hdr, optarg);
			break;
		case 'l':
			/* set lines per page */
			pcnf.p_len = atoi(optarg);
			break;
                case 'n':
			/* enable line numbering */
			pcnf.p_lnum = 1;
                        break;
                case 'o':
			/* set left margin */
			pcnf.p_lmarg = atoi(optarg);
			break;
                case 'p':
			/* preview output on screen */
			strcpy(pcnf.p_dest, "");
			break;
                case 's':
			/* output selected pages */
			strcpy(Pagelist, optarg);
			break;
                case 'w':
                        /* set page width in columns */
			pcnf.p_wid = atoi(optarg);
			break;
		case '?':
			/* unknown option */
			errflag = TRUE;
			break;
		}
	}
	if (errflag == TRUE) {
		pr_help(pgm);
		exit(3);
	}

	/* print the files */
	pr_file(pgm, argc - optind, argv += optind);

	exit(0);
}
