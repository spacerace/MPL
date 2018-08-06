/*
 *	rm -- remove file(s)
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <ctype.h>
#include <io.h>
#include <local\std.h>

main(argc, argv)
int argc;
char *argv[];
{
	int ch;
	BOOLEAN errflag,
		iflag;

	static char pgm[MAXNAME + 1] = { "rm" };
	extern void getpname(char *, char *);
	static void do_rm(char *, char *, BOOLEAN);
	extern int getopt(int, char **, char *);
	extern int optind, opterr;
	extern char *optarg;
	
	/* get program name from DOS (version 3.00 and later) */
	if (_osmajor >= 3)
		getpname(*argv, pgm);

	/* process optional arguments first */
	errflag = iflag = FALSE;
	while ((ch = getopt(argc, argv, "i")) != EOF)
		switch (ch) {
		case 'i':
			/* interactive -- requires confirmation */
			iflag = TRUE;
			break;
		case '?':
			/* say what? */
			errflag = TRUE;
			break;
		}
	argc -= optind;
	argv += optind;

	if (argc <= 0 || errflag == TRUE) {
		fprintf(stderr, "%s [-i] file(s)\n", pgm);
		exit(1);
	}

	/* process remaining arguments */
	for (; argc-- > 0; ++argv)
		do_rm(pgm, *argv, iflag);

	exit(0);
} /* end main() */

/*
 *	do_rm -- remove a file
 */

static void
do_rm(pname, fname, iflag)
char *pname, *fname;
BOOLEAN iflag;
{
	int result = 0;
	struct stat statbuf;
	static BOOLEAN affirm();

	if (iflag == TRUE) {
		fprintf(stderr, "%s (y/n): ", fname);
		if (affirm() == FALSE)
			return;
	}
	if ((result = unlink(fname)) == -1) {
		fprintf(stderr, "%s: ", pname);
		perror(fname);
	}
	return;
}

/*
 *	affirm -- return TRUE if the first character of the
 *	user's response is 'y' or FALSE otherwise
 */

#define MAXSTR	64

static BOOLEAN
affirm()
{
	char line[MAXSTR + 1];
	char *response;

	response = fgets(line, MAXSTR, stdin); 
	return (tolower(*response) == 'y' ? TRUE : FALSE);
}
