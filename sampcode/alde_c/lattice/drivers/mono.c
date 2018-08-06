/*
 *	monochrome device driver
 */

#include "driver.h"

#define	VidSeg	0xB0000000	/*  addr of monochrome video RAM	*/
#define	Attr	0x0700		/*  video attribute mask		*/
#define	Blank	(Attr + ' ')	/*  a blank				*/

char	row,			/*  current row number			*/
	col;			/*  current column number		*/

long	VidAddr;		/*  -> current display character	*/


Init()
	{
	cls();

	((InitParms *)&ReqHdr)->EndAddr = EndAddr();
	ReqHdr.Status = Done;
	}


Output()
	{
	InOutParms	*iop;	/*  our pointer		*/
	long		ta;	/*  -> transfer addr	*/
	int		ctr;	/*  byte count		*/
	char		c;

	iop = (InOutParms *)&ReqHdr;
	ta = iop->Transfer;
	ctr = iop->Count;

	while (ctr--)
		switch (c = PeekB(ta))
			{
			case '\r' :
				doCR();
				break;

			case '\n' :
				doLF();
				break;

			case '\b' :
				doBS();
				break;

			case '\t' :
				doHT();
				break;

			case 0x1A :
				cls();
				break;

			default :
				putone(c);
				break;
			}

	ReqHdr.Status = Done;
	}


OutVerify()
	{
	Output();
	}


OutStatus()
	{
	ReqHdr.Status = Done;
	}


OutFlush()
	{
	ReqHdr.Status = Done;
	}


static cls()
	{
	SetW(VidSeg, 2000, Blank);	/*  clear the screen  */

	row = col = 0;
	VidAddr = VidSeg;
	}


static ScrollUp()
	{
	CopyW(VidSeg + 160, VidSeg, 1920);	/*  move up  */

	SetW(VidSeg + 3840, 80, Blank);		/*  clear last line  */

	VidAddr -= 160;
	row = 24;
	}


static doCR()
	{
	VidAddr -= (col << 1);
	col = 0;
	}


static doLF()
	{
	VidAddr += 160;
	if (++row > 24)
		ScrollUp();
	}


static doBS()
	{
	if (col)
		{
		--col;
		VidAddr -= 2;
		}
	}


static doHT()
	{
	while (col & 7)
		putone(' ');
	}


static putone(c)
	char	c;
	{
	PokeW(Attr + c, VidAddr);
	VidAddr += 2;

	if (++col > 79)
		{
		col = 0;
		doLF();
		}
	}
