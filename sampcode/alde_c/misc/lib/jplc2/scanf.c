/* 1.1  01-08-86						(scanf.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1986		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

/*----------------------------------------------------------------------*/

LOCAL int lastchar;

/************************************************************************/

scanf(fmt, arg)		/* Read stdin using format fmt, and deposit
			   information into arguments in arg list. Return
			   count of items read, or EOF on error.	*/
/*----------------------------------------------------------------------*/
STRING fmt;
unsigned *arg;
{
	int _getc(), unformat();

	lastchar = EOF;
	return unformat(_getc, fmt, &arg);
}

/************************************************************************/
	LOCAL
_getc(forward)		/* Return a character from the stdin.
			   If forward is TRUE, get next character and
			   advance; else, unget the character unless the
			   end has been reached. Return EOF if so.	*/
/*----------------------------------------------------------------------*/
BOOL forward;
{
	LOCAL int lastch = EOF;

	return (lastch = (forward ? (feof(stdin) ? EOF : getchar())
		: ungetc(lastch, stdin)));
}
