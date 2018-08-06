#include <osbind.h>
#include <stdio.h>

#define	MAXBLK		16
#define	FREE		0x00
#define	USED		0x80
#define	NULLBLK		0x80000000L

extern	char	*_mblk[];		/* memory heap pointers */
extern	long	_msiz[];		/* memory heap sizes */

static mergeblk(i)
int i;
/*
 *	Merge adjacent "free" blocks in heap <i>.  Links in the free chain
 *	are guarenteed to be in forward order.
 */
{
	register long n, *p, *q;

	p = _mblk[i];
	if((p = *p) == NULL)			/* empty chain */
		return;
	while(q = p[1]) {
		n = *p;
		if(((char *) p)+n == q)	{	/* free blocks are adjacent */
			p[1] = q[1];		/* re-link free chain */
			*p += *q;		/* adjust block size */
		}
		else
			p = q;
	}
	/* check to see if the entire heap can be returned to the OS */
	q = ((char *) p) + (*p);
	if((_mblk[i] == (p - 1)) && ((*q) == NULLBLK)) {
		Mfree(_mblk[i]);
		_mblk[i] = NULL;
		_msiz[i] = 0L;
	}
}

/*--------------------- Documented Functions ---------------------------*/

free(addr)
register long *addr;
/*
 *	Release the memory block at <addr> back into the free memory pool.
 *	If <addr> doesn't point to a block allocated by calloc(), malloc(),
 *	lalloc() or realloc(), very bad, unpredictable things will happen.
 */
{
	register int i;
	register long *p, *q;

	--addr;					/* point to block header */
	for(i=0; i<MAXBLK; ++i) {
		if((p = _mblk[i]) == NULL)
			continue;		/* skip unavailable blocks */
		if((addr < p) || (addr > ((char *) p)+_msiz[i]))
			continue;		/* block range check */
		while(q = *p) {
			++q;
			if((addr < q) && (addr > p))
				break;
			p = q;
		}
		*((char *) addr) = FREE;	/* link into free chain */
		addr[1] = *p;
		*p = addr;
		mergeblk(i);
		return(TRUE);
	}
	return(FALSE);
}
