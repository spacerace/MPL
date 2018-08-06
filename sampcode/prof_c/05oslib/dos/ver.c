/*
 *	ver -- get the MS-DOS (or PC-DOS) version number
 */

#include <local\doslib.h>

/************************************************************
*  For MS-DOS versions prior to 2.00, the low byte (AL) of
*  the return value is zero.  For versions 2.00 and beyond,
*  the low byte is the major version number and the high
*  byte (AH) is the minor version number.
************************************************************/

int ver()
{
	return(bdos(DOS_VERSION, 0, 0));
}
