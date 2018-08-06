#include <osbind.h>
#include <stdio.h>

int creat(filename, pmode)
register char *filename;
register int pmode;
/*
 *	Create a new file with the given <filename>.  If a file with
 *	the name already exists, it will be truncated to zero bytes.
 *	Since the OS doesn't do this properly, the file is actually
 *	deleted and then re-created.  <pmode> specifies the attributes
 *	initially given to the file.  Valid <pmode> values are:
 *		_CRErw		read/write
 *		_CREro		read only
 *		_CREh		hidden file
 *		_CREs		system file
 *		_CREv		volume label
 *	If _CRErw or _CREv modes are specified, they must be the only
 *	mode given.  Other modes may be combined with the | operator.
 *
 *	Implementation Note:  File is deleted first to avoid a TOS bug
 *	that allows creation of duplicate file names.
 */
{
	register int rv;

	rv = Fdelete(filename);
	if((rv == 0) || (rv == -33))	/* SUCCESS or FILE-NOT-FOUND */
		rv = Fcreate(filename, pmode);
	return(rv);
}
