/* 1.0  01-08-86						(_rowcol.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1986		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

/*----------------------------------------------------------------------*/

/*			EXTERNAL REFERENCES				*/

GLOBAL int outrow[], outcol[];

/************************************************************************/
	VOID
_rowcol(c, fp)		/* update outrow and outcol for character c.	*/

/*----------------------------------------------------------------------*/
FILE *fp;
{
	switch (c)
	{   case '\n':
		++outrow[fp->_unit];
	    case '\r':
		outcol[fp->_unit] = 0;
		break;
	    case '\t':
		while (++outcol[fp->_unit] % TABSTOP)
			;
		break;
	    case '\f':
		outrow[fp->_unit] = 0;
		break;
	    default:
		++outcol[fp->_unit];
	}
}
