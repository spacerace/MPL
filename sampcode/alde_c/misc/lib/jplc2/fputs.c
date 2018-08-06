/* 1.1  01-08-86 						(fputs.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1986		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

/************************************************************************/
	METACHAR
fputs(s, fp)		/* Put the string s out to the FILE fp.  Return
			   NULL if ok, EOF if not.			*/
/*----------------------------------------------------------------------*/
FAST STRING s;
FILE *fp;
{
	while (*s)
		if (putca(*s++, fp) IS EOF)
			return(EOF);

	return NULL;
}
