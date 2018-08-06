/* 1.1  01-08-86						(itoa.c)
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1986		*
 ************************************************************************
 *
 *	These functions are modified versions of itoa() found in Kernighan
 *	and Ritchie, page 60, with modifications as suggested in exercises
 *	3-3 and 3-5, and with returned pointer to result string.
 *
 *----------------------------------------------------------------------*/

#include "defs.h"
#include "stdtyp.h"
#include "errno.h"

/************************************************************************/
	STRING
itoa(s, n, w)	/* return pointer to s, which contains ascii value of
		   n to width |w|, 0-filled if w<0, else space-filled.	*/
/*----------------------------------------------------------------------*/
STRING s;
{
	STRING itoab();

	return itoab(s, n, w, 10);
}

/************************************************************************/
	STRING
itoab(s, n, w, b)	/* convert n to a minimum of |w| ascii characters,
			   base b, and put  in s.  Return pointer to s.
			   0-fill if w<0, else space-fill.		*/
/*----------------------------------------------------------------------*/
STRING s;
{
	LOCAL char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	int i, j;
	BOOL sign;
	char c, fill;

	if (n < 0)
	{	n = -n;
		sign = TRUE;
	}
	else
		sign = FALSE;
	if (w < 0)
	{	fill = '0';
		w = -w;
	}
	else
		fill = ' ';
	i = 0;
	do			/* generate digits in reverse order:	*/
	{	s[i++] = digits[n % b];
	} while (n /= b);
	if (sign)			/* put the sign into the string.*/
		s[j = i++] = '-';
	else
		j = i - 1;
	while (i < w)			/* expand to proper width.	*/
		s[i++] = fill;	/* i is just beyond last filled postion	*/
	if (--i ISNT j AND sign AND fill IS '0')
	{	s[i] = s[j];		/* get the sign.		*/
		s[j] = fill;		/* fill in over it		*/
	}
	s[++i] = NULL;			/* add null terminator	*/
	for (i--, j = 0; j < i; j++, i--) /*  now reverse chars */
	{	c = s[j];
		s[j] = s[i];
		s[i] = c;
	}
	return s;
}
