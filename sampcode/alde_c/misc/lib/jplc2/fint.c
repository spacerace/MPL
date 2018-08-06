/* 1.0  06-27-85						(fint.c)
 ************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/
	double
fint(x)			/* return integer part of x as double		*/

/*----------------------------------------------------------------------*/
double x;
{
	int i, ex2, byte;
	BITS bitmask;
	TBITS *xp;
	LOCAL TBITS mask[8] = 
		{0, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe};

	xp = (char *) &x;

#ifdef ECOSOFT
	ex2 = (((int) xp[0]) - 0x80);/* implied normalized bit, but sign is */
	byte = ex2 / 8 + 1;	/* 	in the normalized place		*/
	bitmask = mask[ex2 & 7];	/* i.e., % 8	*/
#endif

#ifdef AZTEC
	ex2 = (((int) (xp[0] & 0x7f)) - 0x40); /* base 256	*/
	byte = 1 + ex2;
	bitmask = 0;
#endif

#ifdef CPM86
	ex2 = (((int) (((xp[7] << 4) + (xp[6] >> 4)) & 0x7ff)) - 0x3fe + 3);
	byte = 6 - ex2 / 8;	/* has implied normalized bit	*/
	bitmask = mask[ex2 & 7];
#endif

#ifdef UNIVERSE
	ex2 = (((int)(((*xp << 1) + (xp[1] &0x80 ? 1 : 0)) & 0xff)) - 0x80);
	byte = 1 + ex2 / 8; /* has normalized bit, but extended exponent */
	bitmask = mask[ex2 & 7];
#endif
/*\p*/

#ifdef SUN
	ex2 = (int)(((*xp & 0x7f)<<4) + ((xp[1]>>4) & 0xf) - 0x3fe);
	byte = (i = ex2 + 11) / 8;	/* i is location of dec. pt.	*/ 
	bitmask = mask[i & 7];
#endif

	if (ex2 < 1)
		return 0.0;

#ifdef CPM86
	if (byte > -1)
	{	xp[byte] &= bitmask;
		for (i = --byte; i > -1; i--)
			xp[i] = 0;
	}
#else
	if (byte < 8)
	{	xp[byte] &= bitmask;
		for (i = ++byte; i < 8; i++)
			xp[i] = 0;
	}
#endif
	return x;
}
