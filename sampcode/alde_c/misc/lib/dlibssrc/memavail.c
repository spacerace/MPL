#include <osbind.h>
#include <stdio.h>

#define	MAXBLK		16
#define	FREE		0x00
#define	USED		0x80

extern	char	*_mblk[];		/* major memory blocks */

long memavail()
/*
 *	Return the size, in bytes, of the largest block of free memory
 *	available for allocation.  Note that this value is a long.
 */
{
	register int i;
	register long n, tsiz = 0L, **p, *q;

	for(i=0; i<MAXBLK; ++i) {
		if((p = _mblk[i]) == NULL)
			continue;		/* skip unavailable heaps */
		while(q = *p) {
			n = *q;
			if(n > tsiz)		/* largest block so far */
				tsiz = n;
			p = q + 1;
		}
	}
	if((n = Malloc(-1L)) < 1024)
		return(tsiz);
	n -= (1024L + 16L);
	n &= ~0x1FFL;			/* system memory available */
	return((n > tsiz) ? n : tsiz);
}
