/* 1.4  02-10-86						(format.c)
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1986		*
 ************************************************************************/

#include "defs.h"
#include "ctype.h"
#include "stdtyp.h"
#include "stdio.h"

/************************************************************************/

format(xputc, fmt, args)	/* Formatted output of args using fmt;
				   xputc() puts a character on the proper
				   medium.  Return no. of chars printed
				   or EOF if printing fails.		*/
/*----------------------------------------------------------------------*/
FAST int (*xputc)();
FAST STRING fmt;
unsigned *args;
{
	FAST int c;
	int cc, i, k, rjust, filler, precis, width;
	long value;
	STRING cp, ftoa(), ltoab(); 
	char s[MAXLINE];
	double dval;

	for (cc = 0; c = *fmt++; )
	{	if (c ISNT '%')
		{	cc++;
			if ((*xputc)(c) < 0)
				return EOF;
		}
		else
		{	rjust = TRUE;
			filler = ' ';
			precis = MAXLINE;
			if ((c = *fmt++) IS '-')
			{	rjust = FALSE;
				c = *fmt++;
			}
			if (c IS '0')
			{	filler = '0';
				c = *fmt++;
			}
/*\p*/
			if (c IS '*')
			{	width = *args++;
				c = *fmt++;
			}
			else
				for (width = 0; isdigit(c) ; c = *fmt++)
				{	width *= 10;
					width += (c - '0');
				}
			if (width > MAXLINE)
				width = MAXLINE;
			if (c IS '.')
				if ((c = *fmt++) IS '*')
				{	precis = *args++;
					c = *fmt++;
				}
				else
					for (precis = 0; isdigit(c); c = *fmt++)
					{	precis *= 10;
						precis += (c - '0');
					}
			switch(c)
			{   case 'l':
				c = *fmt++;
			    case 'D': case 'O': case 'X': case  'U':
				value = *((long *) args)++;
				break;
			    case 'd':
				value = (int) *args++;
				break;
			    case 'e': case 'f': case 'g':
				dval = *((double *) args)++;
				break;
			    case 'o': case 'x': case 'u': case 's': case 'c':
				value = *args++;
				break;
			}
			if (rjust)
				k = (filler IS '0' AND width) ? -width : width;
			else
				k = 0;
/*\p*/
			switch (TOLOWER(c))
			{   case 'd':
				ltoab(s, value, k, 10);
				break;
			    case 'o':
				ultoab(s, value, k, 8);
				break;
			    case 'u':
				ultoab(s, value, k, 10);
				break;
			    case 'x':
				ultoab(s, value, k, 16);
				break;
			    case 'e':
			    case 'f':
			    case 'g':
				i = (precis IS MAXLINE) ? 6 : precis;
				lpadn(ftoa(s, dval, i, c), filler, k);
				precis = MAXLINE;
				break;
			    case 's':
				if (precis > MAXLINE)
					precis = MAXLINE;
				strncpy(s, (STRING) (unsigned) value, precis);
				break;
			    case 'c':
				c = value;
			    default:
				*s = c;
				s[1] = NULL;
			}
			if ((i = strlen(s)) > precis)
				i = precis;
			i = width - i;
			if (rjust)
				for ( ; i-- > 0; cc++)	
					if ((*xputc)(filler) < 0)
						return EOF;

			for (k = 0, cp = s; *cp AND k < precis; ++k, cc++)
				if((*xputc)(*cp++) < 0)
					return EOF;

			for ( ; i-- > 0; cc++)
				if ((*xputc)(' ') < 0)
					return EOF;
		}
	}
	return cc;
}
/*\p*********************************************************************/
	LOCAL 
lpadn(s, fill, n)	/* left-pad the numeric string s to width n using
			   the fill character.  Return s.		*/
/*----------------------------------------------------------------------*/
FAST STRING s;
FAST int n;
{
	FAST int k, L;

	n -= (L = strlen(s));
	if (n > 0)
	{	for (k = L; k >= 0; k--)	/* move everybody down	*/
			 s[k + n] = s[k];
		for (k = 0; k < n; k++)		/* and fill the holes.	*/
			s[k] = fill;
		if (fill IS '0')
		{	if (s[n] IS '-')
				*s = '-';	/* move the minus sign	*/
			s[n] = fill;		/* plug up the gap.	*/
		}
	}
}

/************************************************************************/
	LOCAL VOID
ultoab(s, n, w, b)	/* Convert unsigned long n to ascii string s, with
			   minimum width w, base b.  Return s.		*/
/*----------------------------------------------------------------------*/
long n;
STRING s;
{
	int low, hb;
	STRING p, ltoab();

	if (n < 0)
	{	low = (int) n & 1;	/* save the low bit of n.	*/
		hb = (b >> 1);		/* hb = half the base b.	*/
		n = LURSHIFT(n, 1);	/* n is now (n - low)/2).	*/
		low += ((int)(n % hb) << 1); /* the low digit base b	*/
		low += (low > 9) ? 'W' - 10 : '0';/* now ascii.	Note 'W'*/
		n /= hb;		/* is 'a' - 10.  Reduce n,	*/
		w -= SGN(w);		/* reduce the width.		*/
	}
	else
		low = NULL;
	p = s + strlen(ltoab(s, n, w, b));
	*p++ = low;
	*p = NULL;
	return;
}
