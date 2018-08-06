SetPixel( x, y )
int	x,y;		/* pixel coordinates */
{
	int	CharCode;
	int	CharScanLine;
	int	BitMask;
	int	CharDefOffset;


        /* the window is 32 characters across */
	CharCode = StartCharCode + (y/Points)*32 + x/8;
	CharScanLine = y % Points;		/* y MOD Points */
	BitMask = 0x80 >> (x % 8);		/* 10000000b SHR (x MOD 8) */

        CharDefOffset = CharCode*2 + CharScanLine*512 + 1;
	CharDefTable[CharDefOffset] |= BitMask;	/* OR the pixel */
}
