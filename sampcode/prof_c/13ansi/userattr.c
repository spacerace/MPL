/*
 *	userattr -- set video attributes to user-specified values
 *	(DOS environment parameters) or to reasonable defaults and
 *	return success or a failure indication for bad attributes
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <local\std.h>
#include <local\ansi.h>
#include <local\ibmcolor.h>

int
userattr(foreground, background, border)
char *foreground, *background, *border;
{
	register char *s;
	static int attrset(POSITION, char *);
	
	if ((s = getenv("FGND")) == NULL)
		s = foreground;
	if (attrset(FGND, s) == -1)
		return FAILURE;

	if ((s = getenv("BKGND")) == NULL)
		s = background;
	if (attrset(BKGND, s) == -1)
		return FAILURE;

	if ((s = getenv("BORDER")) == NULL)
		s = border;
	if (attrset(BDR, s) == -1)
		return FAILURE;

	return SUCCESS;
}

/*
 *	attrset -- parse the color spec and try to set it.
 *	return 0 if OK and -1 upon error (bad color number)
 */
static int
attrset(apos, str)
POSITION apos;
register char *str;
{
	register int attr;
	extern int colornum(char *);
	extern void setattr(POSITION, int);

	if ((attr = colornum(strtok(str, " \t"))) == IBM_BRIGHT)
		attr |= colornum(strtok(NULL, " \t"));
	if (attr >= 0)
		setattr(apos, attr);
	else
		return (-1);
	return (0);
}
