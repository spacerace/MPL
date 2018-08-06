/*
 *	drvpath -- convert a drive name to a full pathname
 */

#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <ctype.h>
#include <local\doslib.h>

char *
drvpath(path)
char path[];	/* path string */
		/* must be large enough to hold a full DOS path + NUL */
{
	union REGS inregs, outregs;
	static int drive(char);

	/* patch root directory onto drive name */
	strcat(path, "\\");

	/* set current directory path for drive from DOS */
	inregs.h.ah = GET_CUR_DIR;
	inregs.h.dl = drive(path[0]);		/* convert to drive number */
	inregs.x.si = (unsigned)&path[3];	/* start of return string */
	intdos(&inregs, &outregs);

	return (outregs.x.cflag ? (char *)NULL : path); 
}

static int
drive(dltr)
char dltr;	/* drive letter */
{
	/* 'A' (or 'a') => 1, 'B' (or 'b') => 2, etc. */
	return (tolower(dltr) - 'a' + 1);
}
