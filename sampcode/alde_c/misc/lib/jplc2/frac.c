/* 1.0  04-27-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/
	double
frac(x)			/* return fractional part of x as double.	*/

/*----------------------------------------------------------------------*/
double x;
{
	double fint();

	return (x - fint(x));
}
