/* Listing 7-2 */

Ellipse( xc, yc, a0, b0 )
int	xc,yc;			/* center of ellipse */
int	a0,b0;			/* semiaxes */
{
	int	x = 0;
	int	y = b0;

	long	a = a0;				/* use 32-bit precision */
	long	b = b0;

	long	Asquared = a * a;		/* initialize values outside */
	long	TwoAsquared = 2 * Asquared;	/*  of loops */
	long	Bsquared = b * b;
	long	TwoBsquared = 2 * Bsquared;

	long	d;
	long	dx,dy;


	d = Bsquared - Asquared*b + Asquared/4L;
	dx = 0;
	dy = TwoAsquared * b;

	while (dx<dy)
	{
	  Set4Pixels( x, y, xc, yc, PixelValue );

	  if (d > 0L)
	  {
	    --y;
	    dy -= TwoAsquared;
	    d -= dy;
	  }

	  ++x;
	  dx += TwoBsquared;
	  d += Bsquared + dx;
	}


	d += (3L*(Asquared-Bsquared)/2L - (dx+dy)) / 2L;

	while (y>=0)
	{
	  Set4Pixels( x, y, xc, yc, PixelValue );

	  if (d < 0L)
	  {
	    ++x;
	    dx += TwoBsquared;
	    d += dx;
	  }

	  --y;
	  dy -= TwoAsquared;
	  d += Asquared - dy;
	}
}


Set4Pixels( x, y, xc, yc, n )	    /* set pixels by symmetry in 4 quadrants */
int	x,y;
int	xc,yc;
int	n;
{
	SetPixel( xc+x, yc+y, n );
	SetPixel( xc-x, yc+y, n );
	SetPixel( xc+x, yc-y, n );
	SetPixel( xc-x, yc-y, n );
}
