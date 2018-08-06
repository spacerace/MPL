/* 1.1  01-08-86						(getf.c)
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1986		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

/************************************************************************/
	double
getf(prompt, check, low, high)	/* Print prompt on stdout, then get and
				   return double input from stdin.  If
				   check is true, verify within bounds.	*/
/*----------------------------------------------------------------------*/
STRING prompt;
BOOL check;
double low, high;
{
	double value, atof();
	char s[MAXLINE];

	FOREVER
	{	value = atof(getns(prompt, s, MAXLINE - 1));
		if (NOT check OR (low <= value AND value <= high))
			break;
		putstr("\nValue out of range, please reenter.\n");
	}
	return (value);
}
