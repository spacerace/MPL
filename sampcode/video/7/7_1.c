/* Listing 7-1 */

Ellipse( xc, yc, a0, b0 )		/* using equation of ellipse */
int	xc,yc;			/* center of ellipse */
int	a0,b0;			/* major and minor axes */
{

	double	x = 0;
	double	y = b0;
	double	Bsquared = (double)b0 * (double)b0;
	double	Asquared = (double)a0 * (double)a0;
	double	sqrt();


	do				/* do while dy/dx >= -1 */
	{
	  y = sqrt( Bsquared - ((Bsquared/Asquared) * x*x) );
	  Set4Pixels( (int)x, (int)y, xc, yc, PixelValue );
	  ++x;
	}
	while ( (x <= a0) && (Bsquared*x < Asquared*y) );


	while (y >= 0)			/* do while dy/dx < -1 */
	{
	  x = sqrt( Asquared - ((Asquared/Bsquared) * y*y) );
	  Set4Pixels( (int)x, (int)y, xc, yc, PixelValue );
	  --y;
	}	
}


Set4Pixels( x, y, xc, yc, n )	/* set pixels in 4 quadrants by symmetry */
int	x,y;
int	xc,yc;
int	n;
{
	SPFunc(xc+x,yc+y,n);
	SPFunc(xc-x,yc+y,n);
	SPFunc(xc+x,yc-y,n);
	SPFunc(xc-x,yc-y,n);
}
