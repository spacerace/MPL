/* Listing 11-6 */

#define	Xmax	640

#define	TRUE	1
#define	FALSE	0

main()
{
	int	x0	= 0;		/* corners of box at 0,0 and 150,100 */
	int	y0	= 0;
	int	x1	= 150;
	int	y1	= 100;
	int	n = 12;			/* pixel value */


	while( x1 < Xmax )			/* slide box right */
	  XORBox( x0++, y0, x1++, y1, n );

	while( x0 > 0 )				/* slide box left */
	  XORBox( --x0, y0, --x1, y1, n );
}


XORBox ( x0, y0, x1, y1, n )
int	x0,y0,x1,y1;		/* pixel coordinates of opposite corners */
int	n;			/* pixel value */
{
	Rectangle( x0, y0, x1, y1, n );			     /* draw the box */
	Rectangle( x0, y0, x1, y1, n );			    /* erase the box */
}


Rectangle( x0, y0, x1, y1, n )
int	x0,y0,x1,y1;
int	n;
{
	Line10( x0, y0, x0, y1, n );
	Line10( x0, y0, x1, y0, n );
	Line10( x1, y1, x0, y1, n );
	Line10( x1, y1, x1, y0, n );
}
