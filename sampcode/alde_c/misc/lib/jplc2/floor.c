/* 1.0  04-27-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/
	double
floor(x)		/* return greatest integer less or equal to x	*/

/*----------------------------------------------------------------------*/
double x;
{
	double fint(), ceil();

	if (x < 0.0)
		return -ceil(-x);
	else	return (fint(x));
}

/************************************************************************/
	double
ceil(x)			/* return least integer greater or equal to x	*/

/*----------------------------------------------------------------------*/
double x;
{
	double modf(), floor();
	int i;

	if (x < 0.0)
		return -floor(-x);
	if (modf(x, &x) > 0.0)
		++x;
	return x;
}
