/* 1.0  12-14-84 						(fwrite.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

/************************************************************************/

fwrite(ptr, ptrsiz, nitems, fp)	/* Append at most nitems of data of type
				   *ptr to FILE fp.  Return number of
				   items actually written.		*/
/*----------------------------------------------------------------------*/
FAST BUFFER ptr;
FILE *fp;
{
	int items;
	FAST int c, i;
	METACHAR putc();

	for (items = 0; items < nitems; ++items)
	{	for (i = ptrsiz; i; --i)
		{	if (putc(*ptr++, fp) IS EOF)
				return items;
		}
	}
	return items;
}
