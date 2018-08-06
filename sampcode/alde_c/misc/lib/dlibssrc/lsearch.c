#include <stdio.h>

char *lfind(key, base, num, size, cmp)
register char *key;
register char *base;
register int num;
register int size;
register int (*cmp)();
/*
 *	Like lsearch(), but do not add elements which are not found.
 */
{
	while(num--) {
		if((cmp)(base, key) == 0)
			return(base);
		base += size;
	}
	return(NULL);
}

char *lsearch(key, base, num, size, cmp)
char *key;
char *base;
int num;
int size;
int (*cmp)();
/*
 *	Perform a linear search for <key> on the data at <base>. The
 *	<num>, <size> and <cmp> parameters are like the corresponding
 *	parameters to qsort().  A pointer to the first matching element
 *	is returned for success, or NULL for failure.  If <key> is not
 *	found, it will be added to the end of the array.
 */
{
	register char *p;

	if(p = lfind(key, base, num, size, cmp))
		return(p);
	blkcpy((base + (size * num)), key, size);
}
