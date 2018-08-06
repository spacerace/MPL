/* 1.0  04-27-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/
	double
round(x)		/* return x rounded to nearest integer in
			   magnitude					*/
/*----------------------------------------------------------------------*/
double x;
{
	double fint();

	if (x < 0.0)
		x -= 0.5;
	else	x += 0.5;
	return (fint(x));
}

