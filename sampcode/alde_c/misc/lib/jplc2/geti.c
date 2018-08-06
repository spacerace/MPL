/* 1.1  01-08-86						(geti.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1986		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

#define MAXDIGITS	6

/************************************************************************/

geti(prompt, check, low, high)	/* Print prompt on stdout, then get and
				   return integer input from stdin.  If
				   check is true, verify within bounds.	*/
/*----------------------------------------------------------------------*/
STRING prompt;
BOOL check;
{
	int value, atoi();
	char s[MAXDIGITS];

	FOREVER
	{	value = atoi(getns(prompt, s, MAXDIGITS - 1));
		if (NOT check OR (low <= value AND value <= high))
			break;
		printf("\nValue out of range, please reenter:\n");
	}
	return (value);
}
