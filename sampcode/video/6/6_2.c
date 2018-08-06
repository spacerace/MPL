/* Listing 6-2 */

Line( x1, y1, x2, y2, n )	/* for lines with slope between -1 and 1 */
int	x1,y1;
int	x2,y2;			/* endpoints */
int	n;			/* pixel value */
{
	int	d,dx,dy;
	int	Aincr,Bincr,yincr;
	int	x,y;


	if (x1 > x2)				/* force x1 < x2 */
	{
	  Swap( &x1, &x2 );
	  Swap( &y1, &y2 );
	}

	if (y2 > y1)				/* determine increment for y */
	  yincr = 1;
	else
	  yincr = -1;

	dx = x2 - x1;				/* initialize constants */
	dy = abs( y2-y1 );
	d = 2 * dy - dx;

	Aincr = 2 * (dy - dx);
	Bincr = 2 * dy;

	x = x1;					/* initial x and y */
	y = y1;

	SetPixel( x, y, n );			/* set pixel at (x1,y1) */

	for (x=x1+1; x<=x2; x++)		/* do from x1+1 to x2 */
	{
	  if (d >= 0)
	  {
	    y += yincr;				/* set pixel A */
	    d += Aincr;
	  }
	  else					/* set pixel B */
	    d += Bincr;

	  SetPixel( x, y, n );
	}
}


Swap( pa, pb )
int	*pa,*pb;
{
	int	t;

	t = *pa;
	*pa = *pb;
	*pb = t;
}
