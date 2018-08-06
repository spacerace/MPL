#include <osbind.h>
#include <stdio.h>

static int	_cfg_ch = _CIOch;	/* getch()/putch() configuration */
unsigned long	_getch = 0L;		/* raw getch() value from OS */

cfg_ch(cfg)
int cfg;
/*
 *	Configure getch()/putch() operation.  The following are legal
 *	values for <cfg> and may be combined with the | operator:
 *		_CIOb		Use BIOS level i/o calls
 *		_CIOch		8-bit character codes only (cf:getch)
 *		_CIOvt		Enable VT-52 escape sequence processing
 *	Return the previous value.  If <cfg> == -1, just return current value.
 */
{
	register int oldcfg;

	oldcfg = _cfg_ch;
	if(cfg != -1)
		_cfg_ch = cfg;
	return(oldcfg);
}

int getch()
/*
 *	Machine dependent console input function.  This function normally
 *	gets a character from the keyboard by calling the GEMDOS "Cconin"
 *	function.  If cfg_ch() is given the _CIOb option, input is gotten
 *	from the BIOS "Bconin" function instead.  The BIOS level functions
 *	don't process ^C, ^S or ^Q, while the GEMDOS functions do.  The
 *	most common use for getch() is when keyboard scan codes are needed
 *	to process special function keys.  The return value from getch()
 *	consists of the scan code in the high-order byte, and the ascii
 *	character code in the low-order byte.  If cfg_ch() is given the
 *	_CIOch option, the return value is always an 8-bit quantity,
 *	either the scan code with the 8th bit set, or the ascii code with
 *	the 8th bit clear.  This is somewhat less informative, since the
 *	scan code form is returned only if the ascii value is 0.  In any
 *	case, the global unsigned long variable "_getch" will contain the
 *	full value returned by the OS.
 */
{
	register unsigned long cc;
	register unsigned int c;

	_getch = cc = (_cfg_ch & _CIOb) ? Bconin(2) : Crawcin();
	if(_cfg_ch & _CIOch) {
		if((c = cc) == 0)		/* null character code */
			c = (cc >> 16) | 0x80;	/* get scan code instead */
		c &= 0xFF;			/* make it 8-bit only */
	}
	else
		c = (0x00FF & cc) | (0xFF00 & (cc >> 8));
	return(c);
}

char putch(c)
register char c;
/*
 *	Machine dependent (typically quite fast) console output function.
 */
{
	if(_cfg_ch & _CIOb)
		if((c < ' ') || (_cfg_ch & _CIOvt))
			Bconout(2, c);
		else
			Bconout(5, c);
	else
		Cconout(c);
	return(c);
}

int getche()
/*
 *	Same as getch() but calls putch() to echo the character.
 */
{
	register char c;

	c = getch();			/* do normal getch() */
	putch((char) _getch);		/* echo from raw OS code */
	return(c);
}

int kbhit()
/*
 *	Machine dependent function to detect if input is waiting for the
 *	getch() function.  Returns non-zero if the console has data ready.
 */
{
	return((_cfg_ch & _CIOb) ? Bconstat(2) : Cconis());
}
