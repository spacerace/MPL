/* 1.0  04-27-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/
	double
modf(x, y)	/* return fractional part of x as double, set *y to
		   integer part of x, also double.			*/
/*----------------------------------------------------------------------*/
double x, *y;
{
	double fint();

	return (x - (*y = fint(x)));
}
