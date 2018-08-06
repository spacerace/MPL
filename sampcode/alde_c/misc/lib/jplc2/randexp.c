/* 1.0  01-22-85 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985		*
 ************************************************************************
 * Computes a sample value from an exponential distribution characterized
 * by unit mean, i.e.,
 *
 *		p(x) = exp(-x)
 *
 * The method is by inversion of the cumulative probability function.  See
 * also
 *
 * Knuth, D. E., FUNDAMENTAL ALGORITHMS, Vol II, "Seminumerical Algorithms"
 * Addison-Wesley Pub. Co.,  page 128.
 *
 */

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/
	double
randexp()			/* Return an exponentially distributed random
			   value with unit mean.			*/
/*----------------------------------------------------------------------*/
{
	double log(), random();

	return (-log(random()));
}
