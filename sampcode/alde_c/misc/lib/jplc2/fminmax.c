/* 1.0  07-06-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"

/************************************************************************/
	double
fmax(x, y)			/* return larger of x and y, double	*/

/*----------------------------------------------------------------------*/
double x, y;
{
	return MAX(x,y);
}

/************************************************************************/
	double
fmin(x, y)			/* return lesser of x and y, double	*/

/*----------------------------------------------------------------------*/
double x, y;
{
	return MIN(x,y);
}
