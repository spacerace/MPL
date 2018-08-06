/* 1.3  01-08-86 						(fprintf.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1986		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

LOCAL FILE *Fp;		/* global FILE pointer for _putc deposit.	*/

/************************************************************************/

fprintf(fp, fmt, args)	/* formatted printf to FILE fp of args.		*/

/*----------------------------------------------------------------------*/
FILE *fp;
STRING fmt;
unsigned args;
{
	int _putc();

	Fp = fp;
	return format(_putc, fmt, &args);
}

/************************************************************************/
	LOCAL
_putc(c)		/* Put character c on global FILE Fp.		*/

/*----------------------------------------------------------------------*/
{
	return putca(c, Fp);
}
