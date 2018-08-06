/* 1.2  01-08-86 						(eprintf.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1986		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

/************************************************************************/

eprintf(fmt, args)	/* formatted print to stderr of args.		*/

/*----------------------------------------------------------------------*/
STRING fmt;
unsigned args;
{
	int _eputc();

	return format(_eputc, fmt, &args);
}

/************************************************************************/
	LOCAL
_eputc(c)		/* print character c on stderr.			*/

/*----------------------------------------------------------------------*/
{
	return eputc(c);
}
