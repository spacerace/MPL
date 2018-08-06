/* 1.2  07-10-85 						(calloc.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/
	char *
calloc(nelem, elsize)	/* Return pointer to storage for nelem elements
			   of size elsize. Initialize storage to zeros.	*/
/*----------------------------------------------------------------------*/
unsigned nelem, elsize;
{
	char *malloc(), *p, *q;
	unsigned i, n_units();

	p = q = malloc(i = nelem * n_units(elsize) * sizeof(HEADER));
	if (q /* ISNT NULL */)
		while (i--)
			*q++ = NULL;
	return p;
}
