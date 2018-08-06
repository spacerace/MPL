/* Listing 10-15 */

#define	Points		14	/* displayed scan lines per character */
#define	StartCharCode	0x80	/* first character code in "window" */
#define	CGenDefSize	32	/* (use 16 for Hercules) */

char far *CRT_MODE = 0x00400049;	/* BIOS video mode number */
int  far *CRT_COLS = 0x0040004A;	/* characters per row */

char far *VideoBuffer;			/* pointer to video buffer */
char far *CharDefTable = 0xA0000000;	/* pointer to char def RAM */
					/* (use 0xB0004000 for Hercules) */
main()
{
	int	i;
	int	CharCode;
	int	CharOffset;
	int	CharScanLine;
	int	CharDefOffset;
	int	Row,Column;


	/* establish alphanumeric mode */

	if (*CRT_MODE == 7)		/* set video buffer pointer */
	  VideoBuffer = 0xB0000000;
	else
	  VideoBuffer = 0xB8000000;

	AlphaModeSet( 8, Points, 8 );


	/* establish a tiled graphics window in the upper left corner */

	CharCode = StartCharCode;

	for ( Row=0; Row<4; Row++ )
	  for ( Column=0; Column< 32; Column++ )
	  {
	    CharOffset = (Row*(*CRT_COLS) + Column) * 2;
	    VideoBuffer[CharOffset] = CharCode++;
	  }		


	/* clear the window */

	CGenModeSet();		/* make character generator RAM addressable */

	for (CharCode=StartCharCode; CharCode<256; CharCode++ )
	  for ( CharScanLine=0; CharScanLine<Points; CharScanLine++ ) 
	  {
	    CharDefOffset = CharCode*CGenDefSize + CharScanLine;
	    CharDefTable[CharDefOffset] = 0;
	  }


	/* draw a few lines */

	for ( i=0; i<256; i++ )		/* horizontal lines */
	{
	  SetPixel( i, 0 );
	  SetPixel( i, 4*Points-1 );
	}

	for ( i=0; i<4*Points-1; i++ )	/* vertical lines */
	{
	  SetPixel( 0, i );
	  SetPixel( 255, i );
	}

	for( i=0; i<Points*4; i++ )	/* diagonal lines */
	{
	  SetPixel( i, i );
	  SetPixel( 255-i, i );
	}

	CGenModeClear();		/* restore alphanumeric mode */

}

SetPixel( x, y )
int	x,y;		/* pixel coordinates */
{
	int	CharCode;
	int	CharScanLine;
	int	BitMask;
	int	CharDefOffset;


	CharCode = StartCharCode + (y/Points)*32 + x/8;
	CharScanLine = y % Points;		/* y MOD Points */
	BitMask = 0x80 >> (x % 8);		/* 10000000b SHR (x MOD 8) */

	CharDefOffset = CharCode*CGenDefSize + CharScanLine;
	CharDefTable[CharDefOffset] |= BitMask;	/* OR the pixel */
}
