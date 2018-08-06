/*
 *	setvmode -- set the video mode
 *	(color/graphics systems only)
 */

#include <dos.h>
#include <local\std.h>
#include <local\bioslib.h>

/***********************************************************
*
* mode # (m)	description
* ------------- ---------------------------------
* PC MODES:
* 	0	40x25 Mono text	(c/g default)
* 	1	40x25 Color text
*	2	80x25 Mono text
*	3	80x25 Color text
*	4	320x200 4-color graphics (med res)
*	5	320x200 Mono graphics (med res)
*	6	640x200 2-color graphics (hi res)
*	7	80x25 on monochrome adapter 
*
* PCjr MODES:
*	8	160x200 16-color graphics
*	9	320x200 16-color graphics
*	10	640x200 4-color fraphics
*
* EGA MODES:
*	13	320x200 16-color graphics
*	14	620x200 16-color graphics
*	15	640x350 mono graphics
*	16	640x350 color graphics (4- or 16-color)
***********************************************************/

int
setvmode(vmode)
unsigned int vmode;	/* user-specified mode number */
{
	union REGS inregs, outregs;

	inregs.h.ah = SET_MODE;
	inregs.h.al = vmode;	/* value not checked */
	int86(VIDEO_IO, &inregs, &outregs);

	/* update video structure */
	getstate();

	return (outregs.x.cflag);
}
