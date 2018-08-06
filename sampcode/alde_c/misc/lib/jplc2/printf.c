/* 1.2  01-08-86						(printf.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1986		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

/************************************************************************/

printf(fmt, args)	/* formatted print to stdout of args.		*/

/*----------------------------------------------------------------------*/
STRING fmt;
unsigned args;
{
	int _putca();

	return format(_putca, fmt, &args);
}

/************************************************************************/
	LOCAL
_putca(c)		/* Print c on stdout.				*/

/*----------------------------------------------------------------------*/
{
	return putchar(c);
}
