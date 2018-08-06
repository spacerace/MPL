/* 1.0  11-12-84
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************
 *	Heapsort subroutine, using an iterative form of the usual
 *	recursive algorithm, such as found in
 *
 *	Aho, A. V., Hopcroft, J. E., and Ullmann, J. D., THE DESIGN AND
 *	ANALYSIS OF ALGORITHMS, Addison-Wesley Pub. Co., pp. 87-92.
 *
 *	The function is written so the array type is unknown to the
 *	algorithm, but is known to comp() and exch().  The comparison
 *	function comp(i, j) must be positive if, and only if, the array
 *	elements indexed by i and j are out of sort.  The function
 *	exch(i, j) must exchange the elements at indices i and j.
 */

#include "defs.h"
#include "stdtyp.h"

int 	(*gcomp)();	/* global compare and exchange functions */
VOID 	(*gexch)();

/************************************************************************/
	VOID
sortH(n, comp, exch)	/* Heapsort array of 0..(n-1) items compared
			   by comp(i,j), exchanged by exch(i,j) 	*/
/*----------------------------------------------------------------------*/
unsigned n;
int (*comp)();
VOID (*exch)();
{
	unsigned i;
	int j;

	gcomp = comp;		/* set functions global to this subroutine */
	gexch = exch;

	for (j = n-- / 2 - 1; j >= 0; j--)	/* build heap. n is now	  */
		heap(j, n);		/* the index of the last element. */
	for (i = n; i > 0; )
	{	(*gexch)(0, i--);
		heap(0, i);
	}
}

/*\p*********************************************************************/
	LOCAL VOID
heap(dad, rightmost)	/* build heap from (dad-1)-st to (rightmost-1)-st
			   elements of the array.			*/
/*----------------------------------------------------------------------*/
unsigned dad, rightmost;
{			/* Array to sort in heapsort algorithm is 1..n, */
			/* but arrays in C go 0..(n-1). Thus the offset.*/

	unsigned son, r;

	FOREVER
	{	if ((son = 2 * dad + 1) > rightmost) /* if no sons remain, */
			return;
		if (son < rightmost)	/* if both right and left sons	*/
			if ((*gcomp)(r = son + 1, son) > 0)
				son = r;
		/* son is now the index of the maximum son-element of dad */
		if ((*gcomp)(son, dad) > 0)
		{	(*gexch)(son, dad);
			dad = son;
		}
		else
			return;
	}
}
