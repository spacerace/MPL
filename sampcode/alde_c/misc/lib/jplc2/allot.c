/* 1.3  02-11-86 						(allot.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985, 86	*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

/*----------------------------------------------------------------------*/

LOCAL HEADER base; 		/* empty list to get started.		*/
LOCAL HEADER *allocp = NULL;	/* last allocated block.		*/

/************************************************************************/
	char *
allot(nbytes)		/* Return pointer to block of memory nbytes in
			   size, or NULL if none available.		*/
/*----------------------------------------------------------------------*/
unsigned nbytes;
{
	HEADER *morecore();	/* Almost straight out of K & R.	*/
	FAST HEADER *p, *q;
	FAST unsigned ru, au;	/* requested units, allotted units.	*/
	unsigned n_units();

	if ((q = allocp) IS NULL)
	{	base.s.ptr = allocp = q = &base;
		base.s.size = 0;
	}
	ru = n_units(nbytes);		/* convert to units	*/
	for (p = q->s.ptr; ; q = p, p = p->s.ptr)
	{	if (p->s.size >= nbytes)
		{	if ((au = n_units(p->s.size)) > ru)
			{	p->s.size = (au -= ru) * sizeof(HEADER);
			 	p += au;
			}
			else		/* allot the whole block.	*/
				q->s.ptr = p->s.ptr;
			allocp = q;
			return (char *) p;
		}
		if ((p IS allocp) AND NOT(p = morecore(ru)))
			return NULL; /* wrapped around free list and
					no memory left.			*/
	}
}
/*\p*********************************************************************/
	VOID
liberate(ap, nbytes)	/* Put block of nbytes pointed to by ap into the
			   available list.				*/
/*----------------------------------------------------------------------*/
char *ap;
unsigned nbytes;
{
	FAST HEADER *p, *q;
	unsigned n_units();

	if (NOT p)
		return;

	p = (HEADER *) ap;			/* point to header	*/
	for (q = allocp; NOT(q < p AND p < q->s.ptr); q = q->s.ptr)
		if (q >= q->s.ptr AND (q < p OR p < q->s.ptr))
			break;	       /* stop if at one end or another	*/

	p->s.size = nbytes;			/* record the size	*/
	if ((p + n_units(p->s.size)) IS q->s.ptr /* join to upper nghbor?*/
	   AND (nbytes % sizeof(HEADER)) IS 0)	/* only if no holes.	*/
	{	p->s.size += q->s.ptr->s.size;
		p->s.ptr = q->s.ptr->s.ptr;
	}
	else
		p->s.ptr = q->s.ptr;
	if (q + n_units(q->s.size) IS p)	/* join to lower nbr? */
	{	q->s.size += p->s.size;
		q->s.ptr = p->s.ptr;
	}
	else
		q->s.ptr = p;
	allocp = q;
}
/*\p*********************************************************************/
	LOCAL HEADER *
morecore(nu)		/* ask system for nu blocks of memory and put on
			   the available list. Return ptr to the block. */
/*----------------------------------------------------------------------*/
unsigned nu;
{
	char *sbrk();
	FAST char *cp;
	FAST HEADER *up;
	FAST unsigned rnu;

	rnu = NALLOC * ((nu + NALLOC - 1) / NALLOC);
	cp = sbrk(nu = rnu * sizeof(HEADER));	/* nu now no. of bytes	*/
	if ((int) cp IS EOF)			/* no space at all	*/
		return NULL;

	up = (HEADER *) cp;
	liberate((char *) up, nu);
	return allocp;
}

/************************************************************************/
	unsigned
n_units(n)		/* return the number of HEADER-sized units
			   required to cover n bytes.			*/
/*----------------------------------------------------------------------*/
unsigned n;
{
	return (n + sizeof(HEADER) - 1) / sizeof(HEADER);
}
