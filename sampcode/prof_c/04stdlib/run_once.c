/*
 *	run_once -- run a program one time and then
 *	"hang" the system to prevent unwanted use
 */

#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <local\std.h>

main(argc, argv)
int argc;
char *argv[];
{
	extern void fatal(char *, char *, int);

	/* skip over the program name */
	++argv;

	/* run the specified command line [pgmname arg(s)] */
	if (spawnvp(P_WAIT, *argv, argv) == -1)
		fatal("run_once", "Error running specified program", 1);
	fprintf(stderr, "Please turn off the power to the computer.\n");

	/* do nothing */
	FOREVER
		;
}
