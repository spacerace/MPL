/* Listing 11-5 */

main()
{
	int	xc	= 400;		/* center of circle */
	int	yc	= 125;
	int	a,b;			/* semimajor and semiminor axes */
	int	n = 12;			/* pixel value */
	int	i;
	float	ScaleFactor = 1.37;	/* for 640x350 16-color mode */


	for( i=0; i<10; i++ )
	  for( a=0; a<100; a++ )
	  {
	    b = (float) a / ScaleFactor;	/* scale semiminor axis */
	    Ellipse10( xc, yc, a, b, n );	/* draw a circle */
	    Ellipse10( xc, yc, a, b, n );	/* draw it again */
	  }
}
