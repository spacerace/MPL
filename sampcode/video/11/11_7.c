/* Listing 11-7 */

#define	Xmax	640			/* screen dimensions in 640x350 mode */
#define	Ymax	350

main()
{
	int	x0	= 150;		/* fixed endpoint at 150,100 */
	int	y0	= 100;
	int	x	= 0;		/* moving endpoint at 0,0 */
	int	y	= 0;
	int	n	= 12;		/* pixel value */


	for( ; x<Xmax; x++ )			/* move right */
	  XORLine( x0, y0, x, y, n );

	for( --x; y<Ymax; y++ )			/* move down */
	  XORLine( x0, y0, x, y, n );

	for( --y; x>=0; --x )			/* move left */
	  XORLine( x0, y0, x, y, n );

	for( x++; y>=0; --y )			/* move up */
	  XORLine( x0, y0, x, y, n );
}


XORLine ( x0, y0, x1, y1, n )
int	x0,y0,x1,y1;		/* endpoints */
int	n;			/* pixel value */
{
	Line10( x0, y0, x1, y1, n );		     /* the line is onscreen */
	Line10( x0, y0, x1, y1, n );		       /* the line is erased */
}
