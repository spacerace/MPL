/*
 *	equipchk -- get equipment list
 */

#include <dos.h>
#include <local\bioslib.h>
#include <local\equip.h>

struct EQUIP Eq;

int
equipchk()
{
	union REGS inregs, outregs;

	/* call BIOS equipment check routine */
	int86(EQUIP_CK, &inregs, &outregs);

	/* extract data from returned data word */
	Eq.nprint  = (outregs.x.ax & 0xC000) / 0x8000;	
	Eq.game_io = ((outregs.x.ax & 0x1000) / 0x1000) ? 1 : 0;
	Eq.nrs232  = (outregs.x.ax & 0x0E00) /0x0200;
	Eq.ndrive  = ((outregs.x.ax & 0x00C0) / 0x0040) + 1;
	Eq.vmode   = (outregs.x.ax & 0x0030) / 0x0010;
	Eq.basemem = ((outregs.x.ax & 0x000C) / 0x0004) + 1;
	Eq.disksys = outregs.x.ax & 0x0001 == 1;

	return (outregs.x.cflag);
}
