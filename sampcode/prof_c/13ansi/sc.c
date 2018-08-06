/*
 *	SetColor (sc) -- set foreground, background, and border
 *	attributes on systems equipped with color display systems
 *
 *	Usage:	sc [foreground [background [border]]]
 *		sc [attribute]
 */

#include <stdio.h>
#include <dos.h>
#include <local\std.h>
#include <local\ansi.h>
#include <local\ibmcolor.h>

main(argc, argv)
int argc;
char **argv;
{
	extern void ansi_tst();
	extern BOOLEAN iscolor();
	extern void setattr(POSITION, int);
	extern void menumode();
	extern parse(int, char **);

	ansi_tst();
	if (iscolor() == FALSE) {
		fprintf(stderr, "\n\nSystem not in a color text mode.\n");
		fprintf(stderr, "Use the MODE command to set the mode.\n");
		exit(2);
	}

	/* process either batch or interactive commands */
	if (argc > 1)
		/* batch mode processing */
		parse(argc, argv);
	else
		/* no command-line args -- interactive mode */
		menumode();

	ANSI_ED;
	exit (0);
}
