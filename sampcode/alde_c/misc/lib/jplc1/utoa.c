/* 1.1  01-08-86						 (utoa.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************
 *
 *	These functions are modified versions of itoa() found in Kernighan
 *	and Ritchie, page 60, with modifications as suggested in exercises
 *	3-4 and 3-5, and with returned pointer to result string.
 *
 *----------------------------------------------------------------------*/

#include "defs.h"
#include "stdtyp.h"
#include "errno.h"

/************************************************************************/
	STRING
utoa(s, n, w)		/* return pointer to string s, which contains ascii
			   value of unsigned n, base 10, min width w.	*/
/*----------------------------------------------------------------------*/
STRING s;
unsigned n;
{
	STRING utoab();

	return utoab(s, n, w, 10);
}

/************************************************************************/
	STRING
utoab(s, n, w, b)	/* convert n to a minimum of w ascii characters,
			   base b, and put  in s.  Return pointer to s.	*/
/*----------------------------------------------------------------------*/
STRING s;
unsigned n;
{
	LOCAL char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	int i, j;
	char c, fill;

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
	while (i < w)			/* expand to proper width.	*/
		s[i++] = fill;	/* i is just beyond last filled postion	*/
	s[i--] = NULL;			/* add null terminator	*/
	for (i, j = 0; j < i; j++, i--) /*  now reverse chars */
	{	c = s[j];
		s[j] = s[i];
		s[i] = c;
	}
	return s;
}
