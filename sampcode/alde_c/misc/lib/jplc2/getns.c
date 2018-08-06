/* 1.2  01-08-86						(getns.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1986		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

/************************************************************************/
	STRING
getns(p, s, n)		/* Prompt with message p on stdout, then get
			   string s of maximum n characters from stdin.
			   Return pointer to s.				*/
/*----------------------------------------------------------------------*/
STRING p, s;
{
	char line[MAXLINE];

	fputs(p, stdout);
	gets(line);
	strncpy(s, line, n);
	return s;
}
