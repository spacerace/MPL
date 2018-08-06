/*
 *	getdrive -- return the number of the default drive
 */

#include <dos.h>
#include <local\doslib.h>

int getdrive()
{
	return (bdos(CURRENT_DISK, 0, 0));
}
