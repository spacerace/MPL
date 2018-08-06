/*
 *	setmydir -- try to change the DOS environment
 */

#include <stdio.h>
#include <stdlib.h>
#include <local\std.h>

main(argc, argv)
int argc;
char **argv;
{
	register char **p;
	static char var[] = { "MYDIR" };
	static char pgm[MAXNAME + 1] = { "setmydir" };
	extern void fatal(char *, char *, int);
	extern void getpname(char *, char *);

	/* use an alias if one is given to this program */
	if (_osmajor >= 3)
		getpname(*argv, pgm);

	/* try to add the MYDIR variable to the environment */
	if (putenv("MYDIR=c:\\mydir") == -1)
		fatal(pgm, "Error changing environment", 1);

	/* display the environment for this process */
	for (p = environ; *p; p++) {
		printf("%s\n", *p);
	}
	
	exit(0);
}
