/*
 *	showenv	-- display the values of any DOS variables
 *	named on the invocation command line
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <local\std.h>

main(argc, argv, envp)
int argc;
char **argv;
char **envp;
{
	register char *ep;
	static char pgm[MAXNAME + 1] = { "showenv" };
	extern void getpname(char *, char *);

	/* use an alias if one is given to this program */
	if (_osmajor >= 3)
		getpname(*argv, pgm);

	/* if no arguments, show full environment list */
	if (argc == 1)
		for (; *envp; ++envp)
			printf("%s\n", *envp);
	else {
		/*
		 *  treat all args as DOS variable names and
		 *  display values of only specified variables
		 */
		++argv;	/* skip past command name */
		--argc;
		while (argc > 0) {
			if ((ep = getenv(strupr(*argv))) == NULL)
				fprintf(stderr, "%s not defined\n", *argv);
			else
				printf("%s=%s\n", *argv, ep);
			++argv;
			--argc;
		}
	}
	exit(0);
}
