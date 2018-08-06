/*
 *	sc_cmds -- display command summary
 */

#include <stdio.h>
#include <local\ansi.h>

void
sc_cmds(fg, bkg, bdr)
int fg, bkg, bdr;
{
	static char *color_xlat[] = {
		"Black (0)", "Blue (1)", "Green (2)", "Cyan (3)",
		"Red (4)", "Magenta (5)", "Brown (6)", "White (7)",
		"Grey (8)", "Light blue (9)", "Light green (10)",
		"Light cyan (11)", "Light red (12)", "Light magenta (13)",
		"Yellow (14)", "Bright white (15)"
	};

	ANSI_CUP(2, 29);
	fputs("*** SetColor (SC) ***", stdout);
	ANSI_CUP(4, 17);
	fputs("Attribute  Decrement  Increment  Current Value", stdout);
	ANSI_CUP(5, 17);
	fputs("---------  ---------  ---------  -------------", stdout);
	ANSI_CUP(6, 17);
	fputs("Foreground    F1         F2", stdout);
	ANSI_CUP(7, 17);
	fputs("Background    F3         F4", stdout);
	ANSI_CUP(8, 17);
	fputs("Border        F5         F6", stdout);
	ANSI_CUP(6, 50);
	fputs(color_xlat[fg], stdout);
	ANSI_CUP(7, 50);
	fputs(color_xlat[bkg], stdout);
	ANSI_CUP(8, 50);
	fputs(color_xlat[bdr], stdout);
	ANSI_CUP(10, 17);
	fputs("Type RETURN to exit. SetColor/Version 2.2", stdout);
	return;
}
