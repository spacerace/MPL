/* Listing 13-1b */

/* draws an n-leaved rose of the form  rho = a * cos(n*theta) */

#define Leaves		(double)11	/* n must be an odd number */

#define	Xmax		640
#define	Ymax		350
#define PixelValue	14
#define	ScaleFactor	(double) 1.37

main()
{
	int	x,y;			/* pixel coordinates */
	double	a;			/* length of the semi-axis */
	double	rho,theta;		/* polar coordinates */

	double	pi = 3.14159265358979;
	double	sin(),cos();

	void SetPixel();


	a = (Ymax / 2) - 1;		/* a reasonable choice for a */

	for (theta=0.0; theta < pi; theta+=0.001)
	{
	  rho = a * cos( Leaves*theta );	/* apply the formula */

	  x = rho * cos( theta );	/* convert to rectangular coords */
	  y = rho * sin( theta ) / ScaleFactor;

	  SetPixel( x+Xmax/2, y+Ymax/2, PixelValue );	/* plot the point */
	}
}
