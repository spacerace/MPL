/* 1.1  01-08-86						(index.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1986		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/
	STRING
index(s, c)		/* return pointer to char c in string s, if it
			   occurs, NULL otherwise.			*/
/*----------------------------------------------------------------------*/
STRING s;
{
	int i;

	while ((i = *s) AND c ISNT i)
		s++;
	return (*s ? s : NULL);
}
