/* Listing 7-5 */

Circle10( xc, yc, xr, yr, n)		/* circles in 640x350 16-color mode */
int	xc,yc;		/* center of circle */
int	xr,yr;		/* point on circumference */
int	n;		/* pixel value */
{
	double	x,y;
	double	sqrt();
	double	Scale10 = 1.37;		/* pixel scaling factor */
	int	a,b;

	x = xr - xc;			/* translate center of ellipse */
	y = (yr - yc) * Scale10;	/*  to origin */

	
	a = sqrt( x*x + y*y );		/* compute major and minor axes */
	b = a / Scale10;

	Ellipse10( xc, yc, a, b, n);	/* draw it */
}
