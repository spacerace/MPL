/*
 *	first_fm - find first file match in work directory
 */

#include <dos.h>
#include <local\doslib.h>

int
first_fm(path, fa)
char *path;	/* pathname of directory */
int fa;		/* attribute(s) of file to match */
{
	union REGS inregs, outregs;

	/* find first matching file */
	inregs.h.ah = FIND_FIRST;
	inregs.x.cx = fa;		
	inregs.x.dx = (unsigned int)path;
	(void)intdos(&inregs, &outregs);

	return (outregs.x.cflag);
}
