/*
 *	timer -- general purpose timer program; uses the
 *	PC's intra-application communication area (ICA) as
 *	a time and date buffer
 */

#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <memory.h>
#include <local\std.h>

#define NBYTES		16
#define ICA_SEG	0x4F
#define MAXTNUM		3
#define TIMEMASK	0x3FFFFFFF
#define FLAGBIT		0x80000000
#define IDBIT		0x40000000

main(argc, argv)
int argc;
char *argv[];
{
	int ch;
	char *cp;	
	int tn;			/* timer number */
	int errflag;		/* error flag */
	int eflag;		/* elapsed time flag */
	int sflag;		/* start timer flag */
	char dest[MAXPATH + 1];	/* destination file name */
	char timestr[MAXLINE];	/* buffer for elapsed time string */
	long now;		/* current time */
	long then;		/* previously recorded time */
	FILE *fout;
	unsigned long tdata[MAXTNUM];

	struct SREGS segregs;

	static char pgm[MAXNAME + 1] = { "timer" };

	static void usage(char *, char *);
	extern char interval(long, char *);
	extern char *getpname(char *, char *);
	extern int getopt(int, char **, char *);
	extern int optind, opterr;
	extern char *optarg;

	if (_osmajor >= 3)
		getpname(*argv, pgm);

	/* process optional arguments */
	fout = stdout;
	tn = 0;
	errflag = eflag = sflag = 0;
	while ((ch = getopt(argc, argv, "0123ef:s")) != EOF) {
		switch (ch) {
		case 'e':
			/* report elapsed timing */
			++eflag;
			break;
		case 'f':
			/* use specified log file or stream */
			strcpy(dest, optarg);
			if ((fout = fopen(dest, "a")) == NULL) {
				fprintf(stderr, "%s: Cannot open %s\n",
					pgm, dest);
				exit(1);
			}
			break;
		case 's':
			/* start (or restart) timing an interval */
			++sflag;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
			/* use specified timer */
			tn = ch - 0x30;
			break;
		case '?':
			/* bad option flag */
			++errflag;
			break;
		}
	}
	argc -= optind;
	argv += optind;

	/* check for errors */
	if (errflag > 0 || argc > 0) 
		usage(pgm, "Bad command line option(s)");

	segread(&segregs);

	/* report current date and time */
	now = time(NULL);
	fprintf(fout, "%s", ctime(&now));

	/* control and report timer data */
	if (eflag) {
		/* report elapsed time for specified timer */
		movedata(ICA_SEG, 0, segregs.ds, tdata, NBYTES);
		then = tdata[tn];
		if ((then & FLAGBIT) != FLAGBIT || (then & IDBIT) != IDBIT) {
			fprintf(stderr, "Timer database corrupted or not set\n");
			exit(1);
		}
		interval(now - (then & TIMEMASK), timestr);
		fprintf(stdout, "Elapsed time = %s\n", timestr);
	}
	if (sflag) {
		/* start (or restart) specified timer */
		movedata(ICA_SEG, 0, segregs.ds, tdata, NBYTES);
		tdata[tn] = (now & TIMEMASK) | FLAGBIT | IDBIT;
		movedata(segregs.ds, tdata, ICA_SEG, 0, NBYTES);
	}
	fputc('\n', fout);

	exit(0);
}

/*
 *	usage -- display a usage message and exit
 *	with an error indication
 */

static void
usage(pname, mesg)
char *pname;
char *mesg;
{
	fprintf(stderr, "%s\n", mesg);
	fprintf(stderr, "Usage: %s [-efs#]\n", pname);
	fprintf(stderr, "\t-e \tshow an elapsed time (must use start first)\n");
	fprintf(stderr, "\t-f file\tappend output to specified file\n");
	fprintf(stderr, "\t-s \tstart (or restart) an interval timer\n");
	fprintf(stderr, "\t-# \tselect a timer (0 to 3; default is 0)\n");
	exit(2);
}
