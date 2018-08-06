/* 1.1  01-08-86						(getl.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1986		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

#define MAXLDIGITS	11	/* Maximum digits in a long, + 1.	*/

/************************************************************************/
	long
getl(prompt, check, low, high)	/* Print prompt on stdout, then get and
				   return long integer input from stdin.
				   If check is true, verify bounds.	*/
/*----------------------------------------------------------------------*/
STRING prompt;
BOOL check;
long low, high;
{
	long value, atol();
	char s[MAXLDIGITS];

	FOREVER
	{	value = atol(getns(prompt, s, MAXLDIGITS-1));
		if (NOT check OR (low <= value AND value <= high))
			break;
		printf("\nValue out of range, please reenter:\n");
	}
	return (value);
}
