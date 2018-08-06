#include <stdio.h>

int	_bsearch;	/* index of element found, or where to insert */

char *bsearch(key, base, num, size, cmp)
register char *key;		/* item to search for */
register char *base;		/* base address */
int num;			/* number of elements */
register int size;		/* element size in bytes */
register int (*cmp)();		/* comparison function */
/*
 *	Perform a binary search for <key> on the sorted data at <base>.
 *	<num>, <size> and <cmp> are like the corresponding parameters
 *	to qsort().  A pointer to the matching element is returned for
 *	success, or NULL for failure.  The global variable "_bsearch"
 *	will contain the index of either the matching element, or the
 *	place where <key> value should be inserted.  The use of "_bsearch"
 *	is not supported by many implementations of bsearch().
 */
{
	register int a, b, c, dir;

	a = 0;
	b = num - 1;
	while(a <= b) {
		c = (a + b) >> 1;	/* == ((a + b) / 2) */
		if (dir = (*cmp)((base + (c * size)), key)) {
			if (dir > 0)
				b = c - 1;
			else /* (dir < 0) */
				a = c + 1;
		}
		else {
			_bsearch = c;
			return(base + (c * size));
		}
	}
	_bsearch = b;	
	return(NULL);
}
