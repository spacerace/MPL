/*
 *	replay -- echo the command-line arguments
 *	to standard output
 */

#include <stdio.h>
#include <stdlib.h>
#include <local\std.h>

main(argc, argv)
int argc;
char **argv;
{
	int i;
	char **p;
	static char pgm[MAXNAME + 1] = { "replay" };

	void getpname(char *, char *);
	
	/* get program name from DOS (version 3.00 and later) */
	if (_osmajor >= 3)
		getpname(*argv, pgm);

	/* check for arguments */
	if (argc == 1)
		exit(1);	/* none given */

	/* echo the argument list, one per line */
	p = argv;
	printf("%s arguments:\n\n", pgm);
	for (--argc, ++argv; argc > 0; --argc, ++argv)
		printf("argv[%d] -> %s\n", argv - p, *argv);
	exit(0);
} /* end main() */
