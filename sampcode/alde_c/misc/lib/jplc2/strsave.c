/* 1.0  07-10-85 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985		*
 ************************************************************************
 *
 *	This is from Kernighan and Ritchie, page 103.
 *
 *---------------------------------------------------------------------*/

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/
	STRING
strsave(s)		/* get enough storage for s and put s there,
			   return pointer to the string, or NULL if no
			   string space 				*/
/*----------------------------------------------------------------------*/
STRING s;
{
	STRING p, malloc();

	if ((p = malloc(strlen(s) + 1)) ISNT NULL)
		strcpy(p, s);
	return p;
}
