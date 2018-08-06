/* 1.1  07-10-85						(realloc.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/
	char *
realloc(ptr, size)	/* Change the size of the block pointed to by ptr,
			   to size bytes.  Return pointer to (possibly
			   moved) block.  Return NULL if no memory
			   available.					*/
/*----------------------------------------------------------------------*/
STRING ptr;
unsigned size;
{
	HEADER *p;
	STRING s, t, malloc();
	int n;

	p = (HEADER *) ptr - 1;	/* point to header	*/
	n = MIN(p->s.size, size);
	free(ptr);
	if ((s = t = malloc(size)) ISNT NULL AND s ISNT ptr)
		while (n-- > 0)
			*s++ = *ptr++;		/* move the data over.	*/
	return s;
}
