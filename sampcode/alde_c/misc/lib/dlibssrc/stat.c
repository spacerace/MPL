#include <osbind.h>
#include <stdio.h>
#include <io.h>

static	STAT	_dta;		/* local DTA buffer */

int access(name, amode)
char *name;
int amode;
/*
 *	Return non-zero if a file with the given <name> can be accessed
 *	in the given <amode>.  Possible <amode> values are:
 *		_ACCe		file/directory exists
 *		_ACCr		file can be read
 *		_ACCw		file can be written
 *		_ACCrw		file can be read and written
 */
{
	register STAT *_pdta;		/* pointer to old DTA */
	register int rv;

	_pdta = Fgetdta();
	Fsetdta(&_dta);
	rv = (Fsfirst(name, amode) == 0);
	Fsetdta(_pdta);
	return(rv);
}

int stat(name, statbuf)
char *name;
STAT *statbuf;
/*
 *	Search for file <name> and load <statbuf> with information
 *	about that file, if it is found.  Return 0 if found, or
 *	ERROR (-1) if no file/directory matched <name>.  Volume
 *	labels are not included in the search.
 */
{
	if(access(name, 0x17)) {
		*statbuf = _dta;
		return(0);
	}
	return(ERROR);
}

long fsize(name)
char *name;
/*
 *	Return the size of the file <name> in bytes.  Note that this
 *	is a long value.  Return -1L if the file is not found.
 */
{
	if(access(name, 0x07))
		return(_dta.S_size);
	return(-1L);
}
