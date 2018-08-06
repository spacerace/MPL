/* 1.0  07-06-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"
#include "errno.h"
#include "mathtyp.h"
#include "mathcons.h"

/************************************************************************/
	double
hypot(x, y)		/* return hypotenuse value of x-y triangle	*/

/*----------------------------------------------------------------------*/
double x, y;
{
	double mx, t;

	if (x < 0.0)
		x = -x;
	if (y < 0.0)
		y = -y;
	mx = MAX(x, y);
	if (x IS 0.00)
		return y;

	if (y IS 0.0)
		return x;

	if (mx < RTHLFINF)		/* we can use fast formula if true */
		return sqrt(x*x + y*y);

	if (x > y)			/* otherwise, must use trig form.  */
		t = atan2(y, x);
	else
		t = atan2(x, y);
	t = cos(t);
	if (mx < INFINLEG)
		return (mx / t);	/* ok because t > 0.707		  */

	t = log(mx) - log(t);		/* but now it's time for drastic  */
	if (t < LOGINFINITY)		/* measures.			  */
		return exp(t);

	errno = ERANGE;			/* but all has failed.		  */
	return INFINITY;
}
