/* 1.0  11-12-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************
 *	Normal-Distribution Random Number Generator.
 *
 * Computes a sample value from a normal distribution characterized by
 * zero mean and unit standard deviation.  Uses "Box-Muller polar method"
 * for calculation of normal deviates,
 *
 * Box, G. E. P., and Muller, M. E., "A Note on the Generation of Normal
 * Deviates," ANNALS OF MATH. STAT., Vol. 29, pp. 610-611, 1958.
 *
 * Refined in 
 *
 * Knuth, D. E., FUNDAMENTAL ALGORITHMS, Vol II, "Seminumerical Algorithms,"
 * Addison-Wesley Pub. Co., page 117.
 *
 */

#include "defs.h"
#include "stdtyp.h"
#include "mathtyp.h"

/************************************************************************/
	double
randnorm()		/* Return a normally distributed random value
			   with zero mean and unit standard deviation.	*/
/*----------------------------------------------------------------------*/
{
	LOCAL BOOL first = TRUE;
	double d, v1, v2;

	if (first--)
	{	do
		{	v1 = 2.0 * random() - 1.0;
			v2 = 2.0 * random() - 1.0;
			/* v1 and v2 are each uniform on [-1, 1] */
			d = v1 * v1 + v2 * v2;
		} while ((d <= 0.0) OR (d >= 1.0));
		/* now, the point (v1, v2) is uniform inside the unit circle,
		 * excluding the origin.
		 */
		d = sqrt(-2.0 * log(d) / d);
		v1 *= d;
		v2 *= d;
		return (v1);
	}
	else
	{	first = TRUE;
		return (v2);
	}
}
