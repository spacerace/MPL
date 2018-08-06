/* 1.0  12-14-84 						(fread.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "stdio.h"

/************************************************************************/

fread(ptr, ptrsiz, nitems, fp)	/* Read nitems of data of type *ptr from
				   FILE fp into block beginning at ptr.
				   Return number of items read.		*/
/*----------------------------------------------------------------------*/
FAST BUFFER ptr;
FILE *fp;
{
	int items;
	FAST int c, i;
	METACHAR getc();

	for (items = 0; items < nitems; ++items)
	{	for (i = ptrsiz; i; --i)
		{	if ((c = getc(fp)) IS EOF)
				return items;

			*ptr++ = c;
		}
	}
	return items;
}
