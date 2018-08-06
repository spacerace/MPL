/*							DUMP GRAPHICS
**************************************************************************
*                       --------------------------                       *
*                      |    Max W. Medley, Jr.    |                      *
*                      |   13550 San Rafael Dr.   |                      *
*                      |  Largo, Florida  33544   |                      *
*                      |   (813) - 595 - 5051     |                      *
*                       --------------------------                       *
*                                                                        *
* Dump graphic screen (high resolution) to an MX-80 printer with the     *
* GRAFTRAX80 rom.  STTY should be used to set all options OFF for $lpt.  *
* All characters must be passed directly to the printer without change.  *
* Default value for tab is used to position the output. If desired, the  *
* position may be changed by either setting a different tab stop or      *
* using spaces (20h).                                                    *
**************************************************************************
*/
#include <stdio.h>

main()
{
	register int row, j, cr;
	unsigned int fp,bit,dot,col;
	char *pd,chr;
	set_extra_segment(0xb800);
	fp = fopen("$lpt", "w");				/* open printer for write */
	putc(0x0a,fp); putc(0x0a,fp); putc(0x0a,fp); putc(0x0a,fp); putc(0x0d,fp);
	putc(0x1b,fp); putc(0x41,fp); putc(0x08,fp);	/* 8/72 lines per inch */
	for(cr=0; cr<640; cr += 8) {			/* Column loop */
		putc(0x09,fp);						/* Tab to column and begin dump */
		putc(0x1b,fp); putc(0x4b,fp); putc(0x90,fp); putc(0x01,fp); /* 480 mode next 400 */
		for(row=199; row>=0; row--) {		/* Row loop */
			chr = 0;						/* Initialize graphics character */
			bit = 0x80;						/* Initialize bit mask */
			for(j=0; j<8; j++) {			/* read 8 dots into graphics character */
				col = cr + j;
				pd = (0x28 * (row & 0xfe)) + (col >> 3);	/* Address within regen area */
				if(row & 1)
					pd += 0x2000;							/* Odd scan */
				dot = (@pd & (0x80 >> (7 & (col & 0xff))));	/* Mask display byte */
				chr += ((dot) ? bit : 0);
				bit >>= 1;					/* Shift mask */
			}
			putc(chr,fp); putc(chr,fp);		/* Two graphics characters */
		}
		putc(0x0a,fp); putc(0x0d,fp);		/* LF - CR */
	}
}
