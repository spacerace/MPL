#include <osbind.h>
#include <stdio.h>

int chmod(filename, pmode)
char *filename;
int pmode;
/*
 *	Change the mode attribute of <filename> to <pmode>.  Values for
 *	<pmode> are the same as for the creat() function.  Returns 0 for
 *	success, or a negative error code.
 */
{
	register int rv;

	if((rv = Fattrib(filename, 1, pmode)) < 0)
		return(rv);
	return(0);
}
