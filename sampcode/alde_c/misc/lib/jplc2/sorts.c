/* 1.0  11-12-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************
 *	Shell sort subroutine, programmed using the algorithm in 
 *
 *	Kernighan, B. W., and Ritchie, D. M., THE C PROGRAMMING LANGUAGE,
 *	Prentice-Hall Software Series, Englewood Cliffs, NJ, 1978,
 *	pages 58 and  117.
 *
 *	It is written so the array type is unknown to the algorithm, but
 *	known to comp() and exch().  The comparison function comp(i, j) 
 *	must be positive if, and only if, the array elements at i and j 
 *	are out of sort.  The function exch(i, j) must exchange array
 *	elements indexed by i and j.
 *
 */

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/
	VOID
sortS(n, comp, exch)		/* Shell sort array of n items compared by
				   comp(i, j), exchanged by exch(i, j).	*/
/*----------------------------------------------------------------------*/
unsigned n;
int (*comp)();
VOID (*exch)();
{
	FAST unsigned i, k, gap;
	FAST int j;

	for (gap = n / 2; gap > 0; gap /= 2)
		for (i = gap; i < n; i++)
		{	j = i - gap;
			while (j >= 0 AND (*comp)(j, (k = j + gap)) > 0)
			{	(*exch)(j, k);
				j -= gap;
			}
		}
}
