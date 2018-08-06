/* Listing 6-1 */

Line( x1, y1, x2, y2, n )
int	x1,y1;			/* endpoint */
int	x2,y2;			/* endpoint */
int	n;			/* pixel value */
{
	int	x,y;
	float	m;		/* slope */
	float	b;		/* y-intercept */


	if (x2 == x1)				/* vertical line */
	{
	  if (y1 > y2)
	   Swap( &y1, &y2 );			/* force y1 < y2 */

	  for (y=y1; y<=y2; y++)		/* draw from y1 to y2 */
	   SetPixel( x1, y, n );

	  return;
	}

	if (x1 > x2)				/* force x1 < x2 */
	{
	  Swap( &x1, &x2 );
	  Swap( &y1, &y2 );
	}


	m = (float)(y2-y1) / (float)(x2-x1);	/* compute m and b */
	b = y1 - (m*x1);

	for (x=x1; x<=x2; x++)			/* draw from x1 to x2 */
	{
	  y = m*x + b;
	  SetPixel( x, y, n );
	}
}

Swap( a, b )			/* exchange values of a and b */
int	*a,*b;
{
	int	t;

	t = *a;
	*a = *b;
	*b = t;
}
