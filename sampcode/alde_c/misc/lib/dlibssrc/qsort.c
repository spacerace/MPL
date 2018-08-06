#include <stdio.h>

char	*_qbuf = NULL;		/* pointer to storage for qsort() */

#define	PIVOT	((i+j)>>1)

static _wqsort(base, lo, hi, cmp)
register int *base;
register int lo;
register int hi;
register int (*cmp)();
{
	int k;
	register int i, j, t;
	register int *p = &k;

	while(hi > lo) {
		i = lo;
		j = hi;
		t = PIVOT;
		*p = base[t];
		base[t] = base[i];
		while(i < j) {
			while(((*cmp)((base+j), p)) > 0)
				--j;
			base[i] = base[j];
			while((i < j) && (((*cmp)((base+i), p)) <= 0))
				++i;
			base[j] = base[i];
		}
		base[i] = *p;
		if((i - lo) < (hi - i)) {
			_wqsort(base, lo, (i - 1), cmp);
			lo = i + 1;
		}
		else {
			_wqsort(base, (i + 1), hi, cmp);
			hi = i - 1;
		}
	}
}

static _lqsort(base, lo, hi, cmp)
register long *base;
register int lo;
register int hi;
register int (*cmp)();
{
	long k;
	register int i, j, t;
	register long *p = &k;

	while(hi > lo) {
		i = lo;
		j = hi;
		t = PIVOT;
		*p = base[t];
		base[t] = base[i];
		while(i < j) {
			while(((*cmp)((base+j), p)) > 0)
				--j;
			base[i] = base[j];
			while((i < j) && (((*cmp)((base+i), p)) <= 0))
				++i;
			base[j] = base[i];
		}
		base[i] = *p;
		if((i - lo) < (hi - i)) {
			_lqsort(base, lo, (i - 1), cmp);
			lo = i + 1;
		}
		else {
			_lqsort(base, (i + 1), hi, cmp);
			hi = i - 1;
		}
	}
}

static _nqsort(base, lo, hi, size, cmp)
register char *base;
register int lo;
register int hi;
register int size;
register int (*cmp)();
{
	register int i, j;
	register char *p = _qbuf;

	while(hi > lo) {
		i = lo;
		j = hi;
		p = (base+size*PIVOT);
		blkcpy(_qbuf, p, size);
		blkcpy(p, (base+size*i), size);
		p = _qbuf;
		while(i < j) {
			while(((*cmp)((base+size*j), p)) > 0)
				--j;
			blkcpy((base+size*i), (base+size*j), size);
			while((i < j) && (((*cmp)((base+size*i), p)) <= 0))
				++i;
			blkcpy((base+size*j), (base+size*i), size);
		}
		blkcpy((base+size*i), p, size);
		if((i - lo) < (hi - i)) {
			_nqsort(base, lo, (i - 1), size, cmp);
			lo = i + 1;
		}
		else {
			_nqsort(base, (i + 1), hi, size, cmp);
			hi = i - 1;
		}
	}
}

qsort(base, num, size, cmp)
char *base;
int num;
int size;
int (*cmp)();
/*
 *	Perform a recursive quick-sort on an array starting at <base>
 *	containing <num> elements of <size> bytes each.  The function
 *	pointed to by <cmp> is used to compare elements.  Pointers to
 *	two items in the array are passed to the function, which must
 *	return a number representing their relationship as follows:
 *		negative	item1 < item2
 *		0		item1 == item2
 *		positive	item1 > item2
 *	The qsort() function requires the use of a temporary data area
 *	that is large enough to hold <size> bytes.  The default space
 *	provided is 128 bytes large.  If your record size is larger than
 *	128 bytes, YOU MUST provide an alternative storage area.  The
 *	global variable "_qbuf" points to the storage qsort() will use.
 *	Setting "_qbuf" to NULL restores use of the internal buffer.
 *	This routine is optimized to avoid N*N sort times for ordered data.
 */
{
	char _qtemp[128];

	if(_qbuf == NULL)
		_qbuf = _qtemp;
	if(size == 2)
		_wqsort(base, 0, num-1, cmp);
	else if(size == 4)
		_lqsort(base, 0, num-1, cmp);
	else
		_nqsort(base, 0, num-1, size, cmp);
	if(_qbuf == _qtemp)
		_qbuf = NULL;
}
