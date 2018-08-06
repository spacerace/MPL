/* 1.4  02-11-86 						(malloc.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985, 86	*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/
	char *
malloc(nbytes)	/* Return pointer to block of memory nbytes in size, or
		   NULL if none available. 				*/
/*----------------------------------------------------------------------*/
FAST unsigned nbytes;
{
	char *allot();
	FAST HEADER *p;
	unsigned n_units();

	nbytes = (1 + n_units(nbytes)) * sizeof(HEADER);
	if (p = (HEADER *)allot(nbytes)) /* round up to align, allocate.*/
	{	p->s.size = nbytes;	/* size field in bytes.		*/
		p++;
	}
	return (char *) p;
}

/************************************************************************/
	VOID
free(ap)		/* put block ap in free memory list.		*/

/*----------------------------------------------------------------------*/
char *ap;
{
	FAST HEADER *p;

	if (p)
	{	p = (HEADER *) ap - 1; /* at header left there by malloc() */
		liberate(p, p->s.size);
	}
}
