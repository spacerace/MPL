/*
 *	iscolor -- return TRUE if a color display system is
 *	in use and is set to one of the text modes
 */

#include <local\std.h>
#include <local\video.h>

BOOLEAN
iscolor()
{
	getstate();
	if (Vmode != CGA_C40 || Vmode != CGA_C80)
		return TRUE;
	return FALSE;
}
