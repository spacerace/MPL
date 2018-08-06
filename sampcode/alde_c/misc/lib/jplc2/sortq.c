/* 1.0  11-12-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************
 *	Quicksort subroutine, using the usual C. A. R. Hoare recursive
 *	algorithm, such as described in 
 *
 *	Knuth, D. E., FUNDAMENTAL ALGORITHMS, Vol. III, "Sorting and 
 *	Searching," Addison-Wesley, pp. 116.
 *
 *	It is written so the array type is unknown to the algorithm, but is 
 *	known to comp() and exch().  The comparison function comp(i, j) 
 *	must be positive if, and only if, the array elements indexed by
 *	i and j are out of sort.  The function exch(i, j) must exchange 
 *	array elements indexed by i and j.
 */

#include "defs.h"
#include "stdtyp.h"

int 	(*gcomp)();	/* global compare and exchange functions */
VOID 	(*gexch)();

/************************************************************************/
	VOID
sortQ(n, comp, exch)		/* Quicksort array of n items compared by
				   comp(i, j), exchanged by exch(i, j)	*/
/*----------------------------------------------------------------------*/
unsigned n;
int (*comp)();
VOID (*exch)();
{
	VOID quick();

	gcomp = comp;		/* set functions global to this subroutine */
	gexch = exch;
	quick(0, n - 1);	/* quicksort from 0 to n-1 */
}

/*\p*********************************************************************/
	LOCAL VOID
quick(low, high)		/* quicksort array from low to high.	*/

/*----------------------------------------------------------------------*/
unsigned low, high;
{
	unsigned j, partition();

	if (low < high)
	{	if ((j = partition(low, high)) IS low)
			quick(low + 1, high);	/* lower segment empty	*/
		else if (j IS high)
			quick(low, j-1);	/* upper segment empty	*/
		else if (j - low < high - j)
		{	quick(low, j - 1);	/* smaller segment first */
			quick(j + 1, high);	/* minimizes stack depth.*/
		}
		else
		{	quick(j + 1, high);
			quick(low, j - 1);
		}
	}
}

/************************************************************************/
	LOCAL unsigned
partition(low, high)	/* partition into lower and upper sets and return
			   index of middle (sorted) element.		*/
/*----------------------------------------------------------------------*/
unsigned low, high;
{
	(*gexch)(low, (low+high)/2);	/* partition on middle element	*/
	do
	{	while (low < high AND (*gcomp)(low, high) <= 0)
			high--;	/* find unsorted higher element	*/
		if (high ISNT low)
		{	(*gexch)(low, high);
			while (low < high AND (*gcomp)(low, high) <= 0)
				low++;	/* find unsorted lower element	*/
			if(low ISNT high)
				(*gexch)(low, high);
		}
	} while(low ISNT high);
	return low;
}
