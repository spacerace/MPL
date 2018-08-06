/* 1.0  07-06-84 
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************
 *
 *	This is the bitcount(n) function in Kernighan and Ritchie, on
 *	page 47.
 */

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/

bitcount(n)		/* count number of 1-bits in n			*/

/*----------------------------------------------------------------------*/
BITS n;
{
	int b;

	for (b = 0; n ISNT 0; n >>= 1)
		if (n & 01)
			b++;
	return (b);
}
