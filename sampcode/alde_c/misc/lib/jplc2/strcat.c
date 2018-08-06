/* 1.0  05-13-85 						(strcat.c)
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/
	STRING
strcat(s1, s2)	/* concatenate strings s1 and s2, return pointer s1	*/

/*----------------------------------------------------------------------*/
STRING s1, s2;
{
	STRING t;

	for (t = s1; *s1; s1++)
		;
	while (*s1++ = *s2++)
		;
	return t;
}
