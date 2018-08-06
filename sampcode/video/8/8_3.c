/* Listing 8-3 */

#define	UP		-1
#define	DOWN		1


LineAdjFill( SeedX, SeedY, D, PrevXL, PrevXR )
int	SeedX,SeedY;		/* seed for current row of pixels */
int	D;			/* direction searched to find current row */
int	PrevXL,PrevXR;		/* endpoints of previous row of pixels */
{
	int	x,y;
	int	xl,xr;
	int	v;

	y = SeedY;		/* initialize to seed coordinates */
	xl = SeedX;
	xr = SeedX;

	ScanLeft( &xl, &y );	/* determine endpoints of seed line segment */
	ScanRight( &xr, &y );

	Line( xl, y, xr, y, FillValue );	/* fill line with FillValue */


/* find and fill adjacent line segments in same direction */

	for (x=xl; x<=xr; x++)		/* inspect adjacent rows of pixels */
	{
	  v = ReadPixel( x, y+D );
	  if ( (v!=BorderValue) && (v!=FillValue) )
	    x = LineAdjFill( x, y+D, D, xl, xr );
	}

/* find and fill adjacent line segments in opposite direction */

	for (x=xl; x<PrevXL; x++)
	{
	  v = ReadPixel( x, y-D );
	  if ( (v!=BorderValue) && (v!=FillValue) )
	    x = LineAdjFill( x, y-D, -D, xl, xr );
	}

	for (x=PrevXR; x<xr; x++)
	{
	  v = ReadPixel( x, y-D );
	  if ( (v!=BorderValue) && (v!=FillValue) )
	    x = LineAdjFill( x, y-D, -D, xl, xr );
	}

	return( xr );
}


ScanLeft( x, y )
int	*x,*y;
{
	int	v;


	do
	{
	  --(*x);			/* move left one pixel */
	  v = ReadPixel( *x, *y );	/* determine its value */
	}
	while ( (v!=BorderValue) && (v!=FillValue) );

	++(*x);		/* x-coordinate of leftmost pixel in row */
}


ScanRight( x, y )
int	*x,*y;
{
	int	v;


	do
	{
	  ++(*x);			/* move right one pixel */
	  v = ReadPixel( *x, *y );	/* determine its value */
	}
	while ( (v!=BorderValue) && (v!=FillValue) );

	--(*x);		/* x-coordinate of rightmost pixel in row */
}
