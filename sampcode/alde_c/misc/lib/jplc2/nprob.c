/* 1.0  04-27-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"
#include "mathcons.h"

/************************************************************************/
	double
nprob(x)		/* Normal probability function, unit variance,
			   zero mean, Pr{z <= x}			*/
/*----------------------------------------------------------------------*/
{
	double ldexp(), erfc();

	return ldexp(erfc(-x / ROOTTWO), -1);
}

/************************************************************************/
	double
nprobc(x)		/* Normal probability complement function, unit
			   variance, zero mean, Pr{z >= x}		*/
/*----------------------------------------------------------------------*/
{
	double erfc();

	return ldexp(erfc(x / ROOTTWO) , -1);
}
