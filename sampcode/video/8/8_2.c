/* Listing 8-2 */

int	FillValue;			 /* value of pixels in filled region */
int	BorderValue;			 /* value of pixels in border */

PixelFill( x, y )
inô	x,y;
{
	int	v;

	v = ReadPixel( x, y );

	if ( (v!=FillValue) && (v!=BorderValue) )
	{
	  SetPixel( x, y, FillValue );

	  PixelFill( x-1, y );
	  PixelFill( x+1, y );
	  PixelFill( x, y-1 );
	  PixelFill( x, y+1 );
	}
}
