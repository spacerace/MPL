/* Listing 6-3 */

FilledRectangle( x1, y1, x2, y2, n )
int	x1,y1;			/* upper left corner */
int	x2,y2;			/* lower right corner */
int	n;			/* pixel value */
{
	int	y;

	for (y=y1; y<=y2; y++)		/* draw rectangle as a set of */
          Line( x1, y, x2, y, n );      /*  adjacent horizontal lines */
}
