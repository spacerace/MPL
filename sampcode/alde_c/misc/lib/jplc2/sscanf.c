/* 1.1  01-08-86						(sscanf.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

/*----------------------------------------------------------------------*/

LOCAL STRING 	scanstr;
LOCAL BOOL 	NOT_EOL;

/************************************************************************/

sscanf(s, fmt, arg)	/* Read string s using format fmt, and deposit
			   information into arguments in arg list. Return
			   count of items read.				*/
/*----------------------------------------------------------------------*/
STRING s, fmt;
int *arg;
{
	int sgetc(), unformat();

	scanstr = s;
	NOT_EOL = TRUE;
	return unformat(sgetc, fmt, &arg);
}

/************************************************************************/
	LOCAL
sgetc(forward)		/* Return a character from the LOCAL scanstr.
			   If forward is TRUE, get next character and
			   advance; else, back up and get one unless the
			   end has been reached. Return EOF if so.	*/
/*----------------------------------------------------------------------*/
BOOL forward;
{
	if (forward)
	{	if (*scanstr)
			return *scanstr++ & 0xff;

		else
			NOT_EOL = FALSE;
	}
	else if (NOT_EOL)
		return *--scanstr & 0xff;

	return EOF;
}
