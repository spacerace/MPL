/* Listing 7-3 */

Set4Pixels( x, y, xc, yc, n )	/* avoids setting the same pixel twice */
int	x,y;
int	xc,yc;
int	n;
{
	if (x!=0)
	{
	  SetPixel( xc+x, yc+y, n );
	  SetPixel( xc-x, yc+y, n );
	  if (y!=0)
	  {
	    SetPixel( xc+x, yc-y, n );
	    SetPixel( xc-x, yc-y, n );
	  }
	}
	else
	{
	  SetPixel( xc, yc+y, n );
	  if (y!=0)
	    SetPixel( xc, yc-y, n );
	}
}
