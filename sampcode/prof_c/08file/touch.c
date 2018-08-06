/*
 *	touch -- update modification time of file(s)
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <sys\utime.h>
#include <io.h>
#include <errno.h>
#include <local\std.h>

/* error return -- big enough not to be mistaken for a bad file count */
#define ERR	0x7FFF

main(argc, argv)
int argc;
char *argv[];
{
	int ch;
	int i;
	int badcount;		/* # of files that can't be updated */
	struct stat statbuf;	/* buffer for stat results */
	BOOLEAN errflag,	/* error flag */
		cflag,		/* creation flag */
		vflag;		/* verbose flag */
	FILE *fp;

	static char pgm[MAXNAME + 1] = { "touch" };
	extern int getopt(int, char **, char *);
	extern int optind, opterr;
	extern char *optarg;
	extern void getpname(char *, char *);
	static void usage(char *);

	/* get program name from DOS (version 3.00 and later) */
	if (_osmajor >= 3)
		getpname(argv[0], pgm);

	/* process optional arguments first */
	errflag = cflag = vflag = FALSE;
	badcount = 0;
	while ((ch = getopt(argc, argv, "cv")) != EOF)
		switch (ch) {
		case 'c':
			/* don't create files */
			cflag = TRUE;
			break;
		case 'v':
			/* verbose -- report activity */
			vflag = TRUE;
			break;
		case '?':
			errflag = TRUE;
			break;
		}
	argc -= optind;
	argv += optind;

	/* check for errors including no file names */
	if (errflag == TRUE || argc <= 0) {
		usage(pgm);
		exit(ERR);
	}

	/* update modification times of files */
	for (; argc-- > 0; ++argv) {
		if (stat(*argv, &statbuf) == -1) {
			/* file doesn't exist */
			if (cflag == TRUE) {
				/* don't create it */
				++badcount;
				continue;
			}
			else if ((fp = fopen(*argv, "w")) == NULL) {
				fprintf(stderr, "%s: Cannot create %s\n",
					pgm, *argv);
				++badcount;
				continue;
			}
			else {
				if (fclose(fp) == EOF) {
					perror("Error closing file");
					exit(ERR);
				}
				if (stat(*argv, &statbuf) == -1) {
					fprintf(stderr, "%s: Cannot stat %s\n", pgm, *argv);
					++badcount;
					continue;
				}
			}
		}			
		if (utime(*argv, NULL) == -1) {
			++badcount;
			perror("Error updating date/time stamp");
			continue;
		}
		if (vflag == TRUE)
			fprintf(stderr, "Touched file %s\n", *argv);
	}

	exit(badcount);
} /* end main() */

/*
 *	usage -- display an informative usage message
 */

static void
usage(pname)
char *pname;
{
	fprintf(stderr, "Usage: %s [-cv] file ...\n", pname);
	fprintf(stderr, "\t-c  Do not create any files\n");
	fprintf(stderr, "\t-v  Verbose mode -- report activities\n");
} /* end usage() */

/*
 *	dummy functions to show how to save a little space
 */

_setenvp()
{
}

#ifndef DEBUG
_nullcheck()
{
}
#endif
