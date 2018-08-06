/* 1.1  01-08-86 						(fscanf.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1986		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

/*----------------------------------------------------------------------*/

LOCAL FILE *Fp;
LOCAL int lastch;

/************************************************************************/

fscanf(fp, fmt, arg)	/* Read FILE fp using format fmt, and deposit
			   information into arguments in arg list. Return
			   count of items read, or EOF on error.	*/
/*----------------------------------------------------------------------*/
FILE *fp;
STRING fmt;
unsigned *arg;
{
	int _getc(), unformat();

	lastch = EOF;
	Fp = fp;
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
	METACHAR getca(), ungetc();

	return (lastch = (forward ? (feof(Fp) ? EOF : getca(Fp))
		: ungetc(lastch, Fp)));
}

