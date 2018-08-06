/* 1.0  07-06-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/

strindex(s, t)		/* return the index of t in s, -1 if none.	*/

/*----------------------------------------------------------------------*/
STRING s, t;
{
	int i, j, k;

	for (i = 0; s[i] ISNT NULL; i++)
	{	for (j = i, k = 0; t[k] ISNT NULL AND s[j] IS t[k]; j++, k++)
			;
		if (t[k] IS NULL)
			return (i);
	}
	return (EOF);
}

/*	This is the index(s,t) function given in Kernighan and Ritchie,
 *	page 67.
 */
