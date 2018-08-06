/* 1.1  01-08-86						(sprintf.c)
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

LOCAL STRING globstr;		/* global string for sputc() deposit	*/

/************************************************************************/

sprintf(str, fmt, args)		/* formatted printf into str string.	*/

/*----------------------------------------------------------------------*/
STRING str, fmt;
unsigned args;
{
	int sputc(), i;

	globstr = str;
	i = format(sputc, fmt, &args);
	*globstr = NULL;
	return i;
}

/************************************************************************/
	LOCAL
sputc(c)		/* Put character c into global string globstr.	*/

/*----------------------------------------------------------------------*/
{
	return (*globstr++ = c) & 0xff;
}
