/* 1.1  01-08-86					     (unformat.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1986		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

#define	NEXT		TRUE	/* Forward direction for getting chars.	*/
#define BACK		FALSE	/* Reverse direction for getting chars.	*/
#define LONGTYP		TRUE	/* Value to signal long or double.	*/
#define NORMTYP		FALSE	/* Value to denote normal width number.	*/
#define SHORTTYP	-TRUE	/* Value to denote short rather than int*/

/*----------------------------------------------------------------------*/

LOCAL int 	width;
LOCAL int 	(*xgetc)();
STRING		index();

/************************************************************************/

unformat(getch, fmt, arglist)	/* Perform scanf() on getch input medium,
				   using format fmt and argument list
				   pointed to by arglist. Returns number of
				   matched and assigned input items, or
				   EOF if input finished before fmt.	*/
/*----------------------------------------------------------------------*/
int (*getch)();			/* Note:  getch(NEXT) performs a	*/
FAST STRING fmt;		/* getc(medium), and getch(BACK) does	*/
int **arglist;			/* an ungetc(c, medium).		*/
{
	FAST int c;
	double nxtfloat(), d;
	long nxtnumber(), val;
	int count, base;
	STRING s;
	BOOL suppress, numtyp;
	char wkbuf[MAXLINE];

	count = 0;
	xgetc = getch;
/*\p*/
	while (c = *fmt++)
	{	if (c IS '%')
		{	numtyp = NORMTYP;
			suppress = FALSE;
			width = MAXLINE;
			if (*fmt IS '*')
			{	++fmt;
				suppress = TRUE;
			}
			if (isdigit(*fmt))
			{	width = 0;
				do
				{	width *= 10;
					width += (*fmt - '0');
				} while (isdigit(*++fmt));
			}
			if (*fmt IS 'l')
			{	numtyp = LONGTYP;
				++fmt;
			}
			if (*fmt IS 'h')
			{	numtyp = SHORTTYP;
				++fmt;
			}
			if (index("DOXEF", c = *fmt++))
				numtyp = LONGTYP;
			switch (c = tolower(c))
			{   case ' ':
				fmt--;
				if (numtyp IS SHORTTYP)
					c = 'd';
				else
					break;
			    case 'd':
			    case 'o':
			    case 'x':
				base = (c IS 'd'? 10 : (c IS 'o' ? 8 : 16));
				if (eatblanks())
					return (count ? count : EOF);

				val = nxtnumber(base);
				if (NOT suppress)
				{	if (numtyp IS LONGTYP)
						*(long *)(*arglist++) = val;
					else if (numtyp IS SHORTTYP)
						*(short *)(*arglist++) = val;
					else
						**arglist++ = val;
					++count;
				}
				break;
/*\p*/
			    case 'e':
			    case 'f':
				if (eatblanks())
					return (count ? count : EOF);

				d = nxtfloat(wkbuf);
				if (NOT suppress)
				{	if (numtyp IS LONGTYP)
						*(double *)(*arglist++) = d;
					else
						*(float *)(*arglist++) = d;
					++count;
				}
				break;
			    case '[':
			    case 's':
				if (c IS '[')
				{	if (*fmt IS '^' OR *fmt IS '~')
					{	numtyp = NORMTYP;
						++fmt;
					}
					else
						numtyp = LONGTYP;
					for (s = wkbuf;(c = *fmt++) ISNT ']';)
						*s++ = c;
					*s = NULL;
				}
				else
				{	numtyp = NORMTYP;
					strcpy(wkbuf, " \t\n");
				}
				if (eatblanks())
					return (count ? count : EOF);

				if (NOT suppress)
					s = (STRING) *arglist++;
				while (width--)
				{	if ((c = (*xgetc)(NEXT)) IS EOF)
						break;
					if (numtyp IS NORMTYP ?
					    (index(wkbuf, c) ISNT NULL) :
					    (index(wkbuf, c)  IS  NULL))
					{	(*xgetc)(BACK);	
						break;
					}
					if (NOT suppress)
						*s++ = c;
				}
				if (NOT suppress)
				{	*s = NULL;
					++count;
				}
				break;
/*\p*/
			    case 'c':
				if ((c = (*xgetc)(NEXT)) IS EOF)
					return (count ? count : EOF);

				if (NOT suppress)
				{	*(STRING)(*arglist++) = c;
					++count;
				}
				break;
			    case '%':
				goto match;
			}
		}
		else if (isspace(c))
		{	if (eatblanks())
				return (count ? count : EOF);
		}
		else
		{
match:			if (eatblanks())
				return (count ? count : EOF);

			if ((*xgetc)(NEXT) ISNT c)
				return count;
		}
	}
	return count;
}

/************************************************************************/
	LOCAL BOOL
eatblanks()		/* Get characters from input medium until no longer
			   whitespace.  Return FALSE normally, or TRUE at 
			   end of line.					*/
/*----------------------------------------------------------------------*/
{
char c;
	while (isspace(c = (*xgetc)(NEXT)))
		;
	if ((*xgetc)(BACK) IS EOF)
		return TRUE;
	else
		return FALSE;
}
/*\p*********************************************************************/
	LOCAL double
nxtfloat(buffer)	/* Get the next ascii e-form or f-form from the
			   input medium, and return its value. Store
			   the ascii form in buffer.			*/
/*----------------------------------------------------------------------*/
STRING buffer;
{
	FAST int c;
	FAST STRING s;
	TBOOL decmlpt, sign, exp;
	double atof();

	s = buffer;
	sign = exp = decmlpt = FALSE;
	while (width--)
	{	c = (*xgetc)(NEXT);
		if (NOT sign AND (c IS '-' OR c IS '+'))
			sign = TRUE;
		else if (NOT decmlpt AND c IS '.')
			sign = decmlpt = TRUE;
		else if (NOT exp AND (c IS 'e' OR c IS 'E'))
		{	sign = FALSE;
			exp = decmlpt = TRUE;
		}
		else if (NOT isdigit(c))
		{	(*xgetc)(BACK);
			break;
		}
		*s++ = c;
	}
	*s = 0;
	return atof(buffer);
}
/*\p*********************************************************************/
	LOCAL long
nxtnumber(base)		/* Get the ascii string of a number with given 
			   base from input medium, return its value.	*/
/*----------------------------------------------------------------------*/
{
	int c;
	long value;
	BOOL minus;
	LOCAL char digits[] = "0123456789abcdef";

	if (width <= 0)
		return 0L;

	value = 0;
	minus = FALSE;
	if ((c = (*xgetc)(NEXT)) IS '-')
	{	minus = TRUE;
		--width;
	}
	else if (c IS '+')
		--width;
	else
		(*xgetc)(BACK);
	while (width--)
	{	c = index(digits, tolower((*xgetc)(NEXT))) - digits;
		if (c < 0 OR c >= base)
		{	(*xgetc)(BACK);
			break;
		}
		value *= base;
		value += c;
	}
	return (minus ? -value : value);
}
