/* 1.0  11-12-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************
 *	Bubble-sort subroutine, using the usual shuttle algorithm,
 *	such as found in JPL's MBASIC(tm) user's manual.  It also appears
 *	in
 *
 *	Tausworthe, Robert, STANDARDIZED DEVELOPMENT OF COMPUTER SOFTWARE,
 *	Part I, Methods, Prentice-Hall, Inc., Englewood Cliffs, NJ, 1976.
 *
 *	It is written so the array type is unknown to the algorithm, but is 
 *	known to comp() and exch().  The comparison function comp(i, j) 
 *	must be positive if, and only if, the array elements indexed by
 *	i and j are out of sort.  The function exch(i, j) must exchange 
 *	elements indexed by i and j.
 *
 */

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/
	VOID
sortB(n, comp, exch)	/* Bubble sort array[0..(n-1)] compared by index
			   function comp(i, j), and exchanged by
			   index function exch(i, j).			*/
/*----------------------------------------------------------------------*/
unsigned n;
int (*comp)();
VOID (*exch)();
{
	FAST unsigned i, j, k, m;

	for (i = 0, j = 1; i < n - 1; i++, j++)
		if ((*comp)(i, j) > 0)
		{	(*exch)(i, j);
			for (k = i; k > 0 AND (*comp)(m=k-1, k) > 0; k--)
				(*exch)(m, k);
		}
}
