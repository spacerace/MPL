/* 1.0  05-07-85						(ltoa.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985		*
 ************************************************************************
 *
 *	These functions are modified versions of itoa() found in Kernighan
 *	and Ritchie, page 60, with modifications of exercises 3-3 and 3-5,
 *	for use with the long data type.
 *
 *----------------------------------------------------------------------*/

#include "defs.h"
#include "stdtyp.h"
#include "errno.h"

/************************************************************************/
	STRING
ltoa(s, n, w)

/*----------------------------------------------------------------------*/
STRING s;
long n;
{
	STRING ltoab();

	return ltoab(s, n, w, 10);
}
/*\p*********************************************************************/
	STRING
ltoab(s, n, w, b)	/* convert n to a minimum of |w| ascii characters,
			   base b, and put in s.  Use 0-fill if w < 0.
			   Return pointer to s.				*/
/*----------------------------------------------------------------------*/
STRING s;
long n;
{
	LOCAL char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	int i, j;
	BOOL sign;
	char c, fill;

	if (n < 0L)
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
		s[i++] = digits[(int)(n % b)];
	while (n /= b);
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